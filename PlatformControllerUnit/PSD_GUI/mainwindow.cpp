#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QStyle>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , esp32_ip("http://192.168.0.225")  // Replace with your ESP32's IP
    , trainApproaching(false)
    , doorProximitySensor(false)
    , hindranceObstacle(false)
    , emergencyRelease(false)
    , fireMode(false)
    , debugLedStatus(false)
{
    ui->setupUi(this);

    // Set up a timer to periodically update the state
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateState);
    timer->start(1000);  // Update every second
}

MainWindow::~MainWindow() {
    delete ui;
}

// Slot implementations
void MainWindow::on_btnTrainApproaching_clicked() {
    trainApproaching = !trainApproaching;
    QString url = esp32_ip + "/setInput?pin=12&value=" + (trainApproaching ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
    ui->btnTrainApproaching->setProperty("state", trainApproaching ? "active" : "inactive");
    ui->btnTrainApproaching->style()->unpolish(ui->btnTrainApproaching);
    ui->btnTrainApproaching->style()->polish(ui->btnTrainApproaching);
}

void MainWindow::on_btnDoorProximitySensor_clicked() {
    doorProximitySensor = !doorProximitySensor;
    QString url = esp32_ip + "/setInput?pin=14&value=" + (doorProximitySensor ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
    ui->btnDoorProximitySensor->setProperty("state", doorProximitySensor ? "active" : "inactive");
    ui->btnDoorProximitySensor->style()->unpolish(ui->btnDoorProximitySensor);
    ui->btnDoorProximitySensor->style()->polish(ui->btnDoorProximitySensor);
}

void MainWindow::on_btnHindranceObstacle_clicked() {
    hindranceObstacle = !hindranceObstacle;
    QString url = esp32_ip + "/setInput?pin=25&value=" + (hindranceObstacle ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
    ui->btnHindranceObstacle->setProperty("state", hindranceObstacle ? "active" : "inactive");
    ui->btnHindranceObstacle->style()->unpolish(ui->btnHindranceObstacle);
    ui->btnHindranceObstacle->style()->polish(ui->btnHindranceObstacle);
}

void MainWindow::on_btnEmergencyRelease_clicked() {
    emergencyRelease = !emergencyRelease;
    QString url = esp32_ip + "/setInput?pin=33&value=" + (emergencyRelease ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
    ui->btnEmergencyRelease->setProperty("state", emergencyRelease ? "active" : "inactive");
    ui->btnEmergencyRelease->style()->unpolish(ui->btnEmergencyRelease);
    ui->btnEmergencyRelease->style()->polish(ui->btnEmergencyRelease);
}

void MainWindow::on_btnFireMode_clicked() {
    fireMode = !fireMode;
    QString url = esp32_ip + "/setInput?pin=1&value=" + (fireMode ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
    ui->btnFireMode->setProperty("state", fireMode ? "active" : "inactive");
    ui->btnFireMode->style()->unpolish(ui->btnFireMode);
    ui->btnFireMode->style()->polish(ui->btnFireMode);
}


void MainWindow::updateInputStates() {
    qDebug() << "Updating input states...";
    QString url = esp32_ip + "/getInputStates";
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString response = QString::fromUtf8(reply->readAll()).trimmed();
            parseInputStatesJSON(response);
        } else {
            qDebug() << "Error retrieving input states:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        // Optionally handle successful response
    } else {
        qDebug() << "Network Error:" << reply->errorString();
    }
    reply->deleteLater();
}

void MainWindow::updateState() {
    QString url = esp32_ip + "/getState";
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString state = QString::fromUtf8(reply->readAll());
            if (state == "IDLE") {
                ui->DoorsClosingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosedWidget->setStyleSheet("background-color: #3d5466; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                if (doorProximitySensor) {
                    ui->TrainDepartWidget->setStyleSheet("background-color: #8b7158; border-radius: 10px;");
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                }
                else {
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
                    ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                }
            } else if (state == "TRAIN_APPROACHING") {
                ui->TrainApproachingWidget->setStyleSheet("background-color: #3d5466; border-radius: 10px;");
                ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            } else if (state == "DOOR_OPENING") {
                ui->DoorsClosedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #8b8958; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainPresentWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
            } else if (state == "DOOR_IS_OPEN") {
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpenWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            } else if (state == "DOOR_CLOSING") {
                ui->DoorsOpenWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosingWidget->setStyleSheet("background-color: #8b7158; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            } else if (state == "OBSTACLE_DETECTED") {
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #915252; border-radius: 10px;");
            } else if (state == "EMERGENCY_OPEN") {
                ui->DoorsClosedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #915252; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            } else if (state == "EMERGENCY_CLOSE") {
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosingWidget->setStyleSheet("background-color: #915252; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            } else if (state == "FIRE_MODE") {
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
                ui->DoorsClosingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpenWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            } else {
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
                ui->DoorsClosingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpenWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            }
        } else {
            ui->DoorsOfflineWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}


void MainWindow::on_btnToggleDebugLED_clicked() {
    debugLedStatus = !debugLedStatus;
    qDebug() << "Button debugLed clicked";
    QString url = esp32_ip + "/toggleDebugLED?state=";
    if (debugLedStatus) {
        url += "ON";
    } else {
        url += "OFF";
    }
    qDebug() << debugLedStatus;
    ui->btnToggleDebugLED->setProperty("state", debugLedStatus ? "active" : "inactive");
    ui->btnToggleDebugLED->style()->unpolish(ui->btnToggleDebugLED);
    ui->btnToggleDebugLED->style()->polish(ui->btnToggleDebugLED);
}

void MainWindow::parseInputStatesJSON(const QString& jsonString) {
    qDebug() << "Received JSON:" << jsonString;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();

        // Extract input states
        auto updateStateLabel = [this, &jsonObj](const QString& inputName, QLabel* label) {
            QJsonObject inputObj = jsonObj.value(inputName).toObject();
            bool softwareState = inputObj.value("software").toBool();  // Only software now
            QString stateText = QString("Software: %1").arg(softwareState ? "HIGH" : "LOW");
            label->setText(stateText);
        };
    } else {
        qDebug() << "Invalid JSON received for input states.";
    }
}







