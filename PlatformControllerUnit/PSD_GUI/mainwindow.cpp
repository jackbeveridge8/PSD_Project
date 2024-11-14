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
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTextEdit>
#include <QWebSocket>

QSerialPort *serialPort;
QTextEdit *terminal;
QByteArray serialBuffer;

#define HENRY


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
#ifdef HENRY
    , esp32_ip("http://192.168.1.114")  // Replace with your ESP32's IP
#endif
#ifdef JACK
    , esp32_ip("http://192.168.0.225")  // Replace with your ESP32's IP
#endif
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

    ui->lbl1->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/Closed.png"));
    ui->lbl2->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/opening.png"));
    ui->lbl3->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/Open.png"));
    ui->lbl4->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/closing.png"));
    ui->lbl5->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/obstructed.png"));
    ui->lbl6->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/Offline.png"));
    ui->lbl7->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/approach.png"));
    ui->lbl8->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/present.png"));
    ui->lbl9->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/depart.png"));
    ui->lbl10->setPixmap(QPixmap("/Users/jackbeveridge/Library/CloudStorage/OneDrive-SwinburneUniversity/2024/PSDGUI/PSD_GUI/images/Offline.png"));

    terminal = ui->terminalText;  // Or a QLabel for simpler messages

    //setupSerialPort(); //use if want to connect terminal messages over usb
    setupWebSocket();

    /*foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Port:" << info.portName();
    }*/
}

MainWindow::~MainWindow() {
    if (serialPort->isOpen()) {
        serialPort->close();  // Ensure the port is closed properly
    }
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
                clearWidgetColours();
                ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosedWidget->setStyleSheet("background-color: #3d5466; border-radius: 10px;");
                if (doorProximitySensor) {
                    ui->TrainDepartWidget->setStyleSheet("background-color: #8b7158; border-radius: 10px;");
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                }
                else {
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
                    ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                }
            } else if (state == "TRAIN_APPROACHING") {
                clearWidgetColours();
                ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosedWidget->setStyleSheet("background-color: #3d5466; border-radius: 10px;");
                ui->TrainApproachingWidget->setStyleSheet("background-color: #3d5466; border-radius: 10px;");
            } else if (state == "DOOR_OPENING") {
                clearWidgetColours();
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #8b8958; border-radius: 10px;");
                if (!emergencyRelease && doorProximitySensor) {
                    ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->TrainPresentWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
                }
            } else if (state == "DOOR_IS_OPEN") {
                clearWidgetColours();
                //ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpenWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
                if (!emergencyRelease && doorProximitySensor) {
                    ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->TrainPresentWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
                }
            } else if (state == "DOOR_CLOSING") {
                clearWidgetColours();
                ui->DoorsClosingWidget->setStyleSheet("background-color: #8b7158; border-radius: 10px;");
                if (!emergencyRelease && doorProximitySensor) {
                    ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                    ui->TrainPresentWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
                }
            } else if (state == "OBSTACLE_DETECTED") {
                clearWidgetColours();
                //ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsObstructedWidget->setStyleSheet("background-color: #915252; border-radius: 10px;");
                ui->TrainPresentWidget->setStyleSheet("background-color: #4d8458; border-radius: 10px;");
            } else if (state == "EMERGENCY_OPEN") {
                clearWidgetColours();
                //ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOpeningWidget->setStyleSheet("background-color: #915252; border-radius: 10px;");
            } else if (state == "EMERGENCY_CLOSE") {
                clearWidgetColours();
                //ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsClosingWidget->setStyleSheet("background-color: #915252; border-radius: 10px;");
            } else if (state == "FIRE_MODE") {
                clearWidgetColours();
                //ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                //ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
            } else {
                clearWidgetColours();
                ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
                ui->DoorsOfflineWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
            }
        } else {
            clearWidgetColours();
            ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
            ui->DoorsOfflineWidget->setStyleSheet("background-color: #2e2d2d; border-radius: 10px;");
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::clearWidgetColours() {
    ui->DoorsClosingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    ui->DoorsObstructedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    ui->DoorsOfflineWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    ui->DoorsClosedWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    ui->DoorsOpeningWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    ui->DoorsOpenWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    //ui->TrainPresentWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    //ui->TrainDepartWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    //ui->PlatformEmptyWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");
    //ui->TrainApproachingWidget->setStyleSheet("background-color: #747474; border-radius: 10px;");

}


void MainWindow::on_btnToggleDebugLED_clicked() {
    debugLedStatus = !debugLedStatus;
    qDebug() << "Button debugLed clicked";
    QString url = esp32_ip + "/toggleDebugLED?state=" + (debugLedStatus ? "ON" : "OFF");
    networkManager->get(QNetworkRequest(QUrl(url)));
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

/*
void MainWindow::setupSerialPort() {
    serialPort = new QSerialPort(this);

    // Select the correct port (use QSerialPortInfo for available ports)
    serialPort->setPortName("cu.SLAB_USBtoUART");  //Replace with the correct port for the Microcontroller, uncomment loop top of page to list all ports on computer
    serialPort->setBaudRate(QSerialPort::Baud115200);

    if (serialPort->open(QIODevice::ReadOnly)) {
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
        qDebug() << "Serial port opened successfully.";
    } else {
        qDebug() << "Failed to open serial port:" << serialPort->errorString();
    }
}

void MainWindow::readSerialData() {
    // Read all available data from the serial port
    serialBuffer.append(serialPort->readAll());

    // Process complete lines
    while (serialBuffer.contains('\n')) {
        int newlineIndex = serialBuffer.indexOf('\n');

        // Extract a single line (up to and including \n)
        QByteArray line = serialBuffer.left(newlineIndex + 1);
        serialBuffer.remove(0, newlineIndex + 1);

        // Append the line to the QTextEdit
        ui->terminalText->append(QString::fromUtf8(line.trimmed()));  // Remove extra spaces/newlines
    }
}
*/

void MainWindow::setupWebSocket() {
    QWebSocket *webSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

    // Handle connection established
    connect(webSocket, &QWebSocket::connected, this, [this]() {
        qDebug() << "WebSocket connected";
        ui->terminalText->append("Connected to DCU Terminal");
    });

    // Handle incoming messages
    connect(webSocket, &QWebSocket::textMessageReceived, this, [this](const QString &message) {
        qDebug() << "Message received:" << message;
        ui->terminalText->append(message.trimmed());
    });

    // Handle connection closed
    connect(webSocket, &QWebSocket::disconnected, this, [this]() {
        qDebug() << "WebSocket disconnected";
        ui->terminalText->append("Disconnected from WebSocket server");
    });

    // Open WebSocket connection
    webSocket->open(QUrl("ws://192.168.0.225/ws"));  // Replace with your ESP32 WebSocket URL
}





