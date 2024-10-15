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
    , debugLedStatus(false)
{
    ui->setupUi(this);

    // Connect button signals to slots
    connect(ui->btnTrainApproaching, &QPushButton::clicked, this, &MainWindow::on_btnTrainApproaching_clicked);
    connect(ui->btnDoorProximitySensor, &QPushButton::clicked, this, &MainWindow::on_btnDoorProximitySensor_clicked);
    connect(ui->btnHindranceObstacle, &QPushButton::clicked, this, &MainWindow::on_btnHindranceObstacle_clicked);
    connect(ui->btnEmergencyRelease, &QPushButton::clicked, this, &MainWindow::on_btnEmergencyRelease_clicked);
    connect(ui->btnToggleDebugLED, &QPushButton::clicked, this, &MainWindow::on_btnToggleDebugLED_clicked);

    requestDebugLEDState();


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
    if (trainApproaching == false) {
        ui->lblTrainIsApproachingState->setText("false");
    } else {
        ui->lblTrainIsApproachingState->setText("true");
    }

    QString url = esp32_ip + "/setInput?pin=13&value=" + (trainApproaching ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::on_btnDoorProximitySensor_clicked() {
    doorProximitySensor = !doorProximitySensor;
    if (doorProximitySensor == false) {
        ui->lblDoorProximitySensorState->setText("false");
    } else {
        ui->lblDoorProximitySensorState->setText("true");
    }
    QString url = esp32_ip + "/setInput?pin=14&value=" + (doorProximitySensor ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::on_btnHindranceObstacle_clicked() {
    hindranceObstacle = !hindranceObstacle;
    if (hindranceObstacle == false) {
        ui->lblHindranceObstacleState->setText("false");
    } else {
        ui->lblHindranceObstacleState->setText("true");
    }
    QString url = esp32_ip + "/setInput?pin=25&value=" + (hindranceObstacle ? "HIGH" : "LOW");
    networkManager->get(QNetworkRequest(QUrl(url)));
}

void MainWindow::on_btnEmergencyRelease_clicked() {
    emergencyRelease = !emergencyRelease;
    if (emergencyRelease == false) {
        ui->lblEmergencyReleaseState->setText("false");
    } else {
        ui->lblEmergencyReleaseState->setText("true");
    }
    QString url = esp32_ip + "/setInput?pin=33&value=" + (emergencyRelease ? "HIGH" : "LOW");
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
                ui->lblCurrentState->setText("CLOSED");
            } else if (state == "TRAIN_APPROACHING") {
                ui->lblCurrentState->setText("CLOSED");
            } else if (state == "DOOR_OPENING") {
                ui->lblCurrentState->setText("OPENING");
            } else if (state == "DOOR_IS_OPEN") {
                ui->lblCurrentState->setText("OPEN");
            } else if (state == "DOOR_CLOSING") {
                ui->lblCurrentState->setText("CLOSING");
            } else if (state == "OBSTACLE_DETECTED") {
                ui->lblCurrentState->setText("OBSTACLE DETECTED");
            } else if (state == "EMERGENCY_OPEN") {
                ui->lblCurrentState->setText("EMERGENCY OPEN");
            } else if (state == "EMERGENCY_CLOSE") {
                ui->lblCurrentState->setText("EMERGENCY CLOSE");
            } else if (state == "FIRE_MODE") {
                ui->lblCurrentState->setText("FIRE MODE");
            } else {
                ui->lblCurrentState->setText("Error...");
            }
            updateCircleLightStatus();
        } else {
            ui->lblCurrentState->setText("Error retrieving state");
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::updateCircleLightStatus() {
    QString state = ui->lblCurrentState->text();
    if (state == "CLOSED") {
        ui->CircleLightStatus->setStyleSheet("background-color: red; border-radius: 15px;");
    } else if (state == "OPENING" || state == "OPEN") {
        ui->CircleLightStatus->setStyleSheet("background-color: green; border-radius: 15px;");
    } else if (state == "OBSTACLE DETECTED") {
        ui->CircleLightStatus->setStyleSheet("background-color: orange; border-radius: 15px;");
    } else {
        ui->CircleLightStatus->setStyleSheet("background-color: red; border-radius: 15px;");
    }
}


void MainWindow::on_btnToggleDebugLED_clicked() {
    static bool debugLEDState = false;
    debugLEDState = !debugLEDState;

    ui->btnToggleDebugLED->setText(debugLedStatus ? "Turn Debug LED Off" : "Turn Debug LED On");

    QString url = esp32_ip + "/toggleDebugLED?state=" + (debugLedStatus ? "ON" : "OFF");
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Optionally handle successful response
        } else {
            qDebug() << "Network Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
    debugLedStatus = !debugLedStatus;
}

void MainWindow::requestDebugLEDState() {
    QString url = esp32_ip + "/getDebugLEDState";
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QString response = QString::fromUtf8(reply->readAll()).trimmed();
            bool debugLEDState = (response == "ON");
            ui->btnToggleDebugLED->setText(debugLEDState ? "Turn Debug LED Off" : "Turn Debug LED On");
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
            bool hardwareState = inputObj.value("hardware").toBool();
            bool softwareState = inputObj.value("software").toBool();
            QString stateText = QString("Hardware: %1 | Software: %2")
                                    .arg(hardwareState ? "HIGH" : "LOW")
                                    .arg(softwareState ? "HIGH" : "LOW");
            label->setText(stateText);
        };

        // Update labels
        updateStateLabel("trainIsApproaching", ui->lblTrainIsApproachingState);
        updateStateLabel("doorProximitySensor", ui->lblDoorProximitySensorState);
        updateStateLabel("hindranceObstacleDetection", ui->lblHindranceObstacleState);
        updateStateLabel("emergencyReleaseButton", ui->lblEmergencyReleaseState);
    } else {
        qDebug() << "Invalid JSON received for input states.";
    }
}

void MainWindow::applyShadowEffect() {
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20);  // Customize this
    shadowEffect->setOffset(10, 10);    // Customize this
    shadowEffect->setColor(Qt::black);  // You can also use QColor with RGB or hex

    ui->DoorDisplayWidget->setGraphicsEffect(shadowEffect);
}



