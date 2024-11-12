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
#include <QGraphicsDropShadowEffect>

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

    // Connect button signals to slots
    connect(ui->btnTrainApproaching, &QPushButton::clicked, this, &MainWindow::on_btnTrainApproaching_clicked);
    connect(ui->btnDoorProximitySensor, &QPushButton::clicked, this, &MainWindow::on_btnDoorProximitySensor_clicked);
    connect(ui->btnHindranceObstacle, &QPushButton::clicked, this, &MainWindow::on_btnHindranceObstacle_clicked);
    connect(ui->btnEmergencyRelease, &QPushButton::clicked, this, &MainWindow::on_btnEmergencyRelease_clicked);
    connect(ui->btnToggleDebugLED, &QPushButton::clicked, this, &MainWindow::on_btnToggleDebugLED_clicked);
    connect(ui->btnFireMode, &QPushButton::clicked, this, &MainWindow::on_btnFireMode_clicked);

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
}

void MainWindow::on_btnDoorProximitySensor_clicked() {
    doorProximitySensor = !doorProximitySensor;
    /*if (doorProximitySensor == false) {
        ui->lblDoorProximitySensorState->setText("false");
    } else {
        ui->lblDoorProximitySensorState->setText("true");
    }*/
    QString url = esp32_ip + "/setInput?pin=14&value=" + (doorProximitySensor ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::on_btnHindranceObstacle_clicked() {
    hindranceObstacle = !hindranceObstacle;
    /*if (hindranceObstacle == false) {
        ui->lblHindranceObstacleState->setText("false");
    } else {
        ui->lblHindranceObstacleState->setText("true");
    }*/
    QString url = esp32_ip + "/setInput?pin=25&value=" + (hindranceObstacle ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::on_btnEmergencyRelease_clicked() {
    emergencyRelease = !emergencyRelease;
    /*if (emergencyRelease == false) {
        ui->lblEmergencyReleaseState->setText("false");
    } else {
        ui->lblEmergencyReleaseState->setText("true");
    }*/
    QString url = esp32_ip + "/setInput?pin=33&value=" + (emergencyRelease ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::on_btnFireMode_clicked() {
    fireMode = !fireMode;
    QString url = esp32_ip + "/setInput?pin=1&value=" + (fireMode ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
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
                ui->PlatformEmptyWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
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
    QString url = esp32_ip + "/toggleDebugLED?state=" + (debugLedStatus ? "ON" : "OFF");
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Optionally, parse ESP32 response here
            QString response = QString::fromUtf8(reply->readAll()).trimmed();
            debugLedStatus = (response == "ON");
        } else {
            qDebug() << "Network Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
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

        // Update labels
        /*
        updateStateLabel("trainIsApproaching", ui->lblTrainIsApproachingState);
        updateStateLabel("doorProximitySensor", ui->lblDoorProximitySensorState);
        updateStateLabel("hindranceObstacleDetection", ui->lblHindranceObstacleState);
        updateStateLabel("emergencyReleaseButton", ui->lblEmergencyReleaseState);
        */
    } else {
        qDebug() << "Invalid JSON received for input states.";
    }
}







