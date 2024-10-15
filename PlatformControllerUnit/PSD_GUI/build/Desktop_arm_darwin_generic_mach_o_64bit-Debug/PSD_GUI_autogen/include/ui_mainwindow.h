/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *Background;
    QWidget *DoorDisplayWidget;
    QLabel *txtDoorState;
    QLabel *lblCurrentState;
    QWidget *CircleLightStatus;
    QWidget *TopBanner;
    QLabel *label_2;
    QPushButton *btnTrainApproaching;
    QPushButton *btnDoorProximitySensor;
    QPushButton *btnHindranceObstacle;
    QPushButton *btnEmergencyRelease;
    QLabel *lblTrainIsApproachingState;
    QLabel *lblDoorProximitySensorState;
    QLabel *lblHindranceObstacleState;
    QLabel *lblEmergencyReleaseState;
    QPushButton *btnToggleDebugLED;
    QWidget *DoorDisplayWidget_2;
    QWidget *TopBanner_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        MainWindow->setStyleSheet(QString::fromUtf8("#MainWindow {\n"
"	background-color: white;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8(""));
        Background = new QWidget(centralwidget);
        Background->setObjectName("Background");
        Background->setGeometry(QRect(0, 0, 800, 601));
        Background->setStyleSheet(QString::fromUtf8("#Background {\n"
"	Background-color: white;\n"
"}"));
        DoorDisplayWidget = new QWidget(Background);
        DoorDisplayWidget->setObjectName("DoorDisplayWidget");
        DoorDisplayWidget->setGeometry(QRect(20, 90, 371, 151));
        DoorDisplayWidget->setStyleSheet(QString::fromUtf8("#DoorDisplayWidget {\n"
"	Background-color: #f3e2ce;\n"
"	border-radius: 10px;\n"
"}"));
        txtDoorState = new QLabel(DoorDisplayWidget);
        txtDoorState->setObjectName("txtDoorState");
        txtDoorState->setGeometry(QRect(10, 10, 181, 16));
        QFont font;
        font.setFamilies({QString::fromUtf8("Helvetica Neue")});
        font.setPointSize(14);
        font.setBold(true);
        txtDoorState->setFont(font);
        txtDoorState->setStyleSheet(QString::fromUtf8("#txtDoorState {\n"
"	color: black;\n"
"}"));
        lblCurrentState = new QLabel(DoorDisplayWidget);
        lblCurrentState->setObjectName("lblCurrentState");
        lblCurrentState->setGeometry(QRect(70, 50, 231, 41));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Helvetica Neue")});
        font1.setPointSize(24);
        lblCurrentState->setFont(font1);
        lblCurrentState->setStyleSheet(QString::fromUtf8("#lblCurrentState {\n"
"	color: black;\n"
"}"));
        lblCurrentState->setAlignment(Qt::AlignmentFlag::AlignCenter);
        CircleLightStatus = new QWidget(DoorDisplayWidget);
        CircleLightStatus->setObjectName("CircleLightStatus");
        CircleLightStatus->setGeometry(QRect(170, 100, 30, 30));
        CircleLightStatus->setStyleSheet(QString::fromUtf8("#CircleLightStatus {\n"
"	background-color: red;\n"
"	border-radius: 15px;\n"
"}\n"
""));
        CircleLightStatus->raise();
        lblCurrentState->raise();
        txtDoorState->raise();
        TopBanner = new QWidget(Background);
        TopBanner->setObjectName("TopBanner");
        TopBanner->setGeometry(QRect(0, 0, 801, 71));
        TopBanner->setAutoFillBackground(false);
        TopBanner->setStyleSheet(QString::fromUtf8("#TopBanner {\n"
"	Background-color: white;\n"
"}"));
        label_2 = new QLabel(TopBanner);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 10, 411, 51));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Helvetica Neue")});
        font2.setPointSize(48);
        label_2->setFont(font2);
        label_2->setStyleSheet(QString::fromUtf8("#label_2 {\n"
"	color: black;\n"
"}"));
        btnTrainApproaching = new QPushButton(Background);
        btnTrainApproaching->setObjectName("btnTrainApproaching");
        btnTrainApproaching->setGeometry(QRect(20, 280, 191, 32));
        btnTrainApproaching->setStyleSheet(QString::fromUtf8("#btnTrainApproaching {\n"
"    background-color: rgb(237, 123, 1); \n"
"    color: white; \n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#btnTrainApproaching:hover {\n"
"    background-color: rgb(163, 85, 2); \n"
"}\n"
"\n"
"#btnTrainApproaching:pressed {\n"
"    background-color: rgb(107, 55, 0); \n"
"}"));
        btnDoorProximitySensor = new QPushButton(Background);
        btnDoorProximitySensor->setObjectName("btnDoorProximitySensor");
        btnDoorProximitySensor->setGeometry(QRect(20, 330, 191, 32));
        btnDoorProximitySensor->setStyleSheet(QString::fromUtf8("#btnDoorProximitySensor {\n"
"    background-color: rgb(237, 123, 1); \n"
"    color: white; \n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#btnDoorProximitySensor:hover {\n"
"    background-color: rgb(163, 85, 2); \n"
"}\n"
"\n"
"#btnDoorProximitySensor:pressed {\n"
"    background-color: rgb(107, 55, 0); \n"
"}"));
        btnHindranceObstacle = new QPushButton(Background);
        btnHindranceObstacle->setObjectName("btnHindranceObstacle");
        btnHindranceObstacle->setGeometry(QRect(20, 380, 191, 32));
        btnHindranceObstacle->setStyleSheet(QString::fromUtf8("#btnHindranceObstacle {\n"
"    background-color: rgb(237, 123, 1); \n"
"    color: white; \n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#btnHindranceObstacle:hover {\n"
"    background-color: rgb(163, 85, 2); \n"
"}\n"
"\n"
"#btnHindranceObstacle:pressed {\n"
"    background-color: rgb(107, 55, 0); \n"
"}"));
        btnEmergencyRelease = new QPushButton(Background);
        btnEmergencyRelease->setObjectName("btnEmergencyRelease");
        btnEmergencyRelease->setGeometry(QRect(20, 430, 191, 32));
        btnEmergencyRelease->setStyleSheet(QString::fromUtf8("#btnEmergencyRelease {\n"
"    background-color: rgb(237, 123, 1); \n"
"    color: white; \n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#btnEmergencyRelease:hover {\n"
"    background-color: rgb(163, 85, 2); \n"
"}\n"
"\n"
"#btnEmergencyRelease:pressed {\n"
"    background-color: rgb(107, 55, 0); \n"
"}"));
        lblTrainIsApproachingState = new QLabel(Background);
        lblTrainIsApproachingState->setObjectName("lblTrainIsApproachingState");
        lblTrainIsApproachingState->setGeometry(QRect(240, 290, 58, 16));
        lblTrainIsApproachingState->setStyleSheet(QString::fromUtf8("#lblTrainIsApproachingState {\n"
"	color: black;\n"
"}"));
        lblDoorProximitySensorState = new QLabel(Background);
        lblDoorProximitySensorState->setObjectName("lblDoorProximitySensorState");
        lblDoorProximitySensorState->setGeometry(QRect(240, 340, 58, 16));
        lblDoorProximitySensorState->setStyleSheet(QString::fromUtf8("#lblDoorProximitySensorState {\n"
"	color: black;\n"
"}"));
        lblHindranceObstacleState = new QLabel(Background);
        lblHindranceObstacleState->setObjectName("lblHindranceObstacleState");
        lblHindranceObstacleState->setGeometry(QRect(240, 390, 58, 16));
        lblHindranceObstacleState->setStyleSheet(QString::fromUtf8("#lblHindranceObstacleState {\n"
"	color: black;\n"
"}"));
        lblEmergencyReleaseState = new QLabel(Background);
        lblEmergencyReleaseState->setObjectName("lblEmergencyReleaseState");
        lblEmergencyReleaseState->setGeometry(QRect(240, 440, 58, 16));
        lblEmergencyReleaseState->setStyleSheet(QString::fromUtf8("#lblEmergencyReleaseState {\n"
"	color: black;\n"
"}"));
        btnToggleDebugLED = new QPushButton(Background);
        btnToggleDebugLED->setObjectName("btnToggleDebugLED");
        btnToggleDebugLED->setGeometry(QRect(610, 290, 161, 32));
        btnToggleDebugLED->setStyleSheet(QString::fromUtf8("#btnToggleDebugLED {\n"
"    background-color: rgb(237, 123, 1); \n"
"    color: white; \n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#btnToggleDebugLED:hover {\n"
"    background-color: rgb(163, 85, 2); \n"
"}\n"
"\n"
"#btnToggleDebugLED:pressed {\n"
"    background-color: rgb(107, 55, 0); \n"
"}"));
        DoorDisplayWidget_2 = new QWidget(Background);
        DoorDisplayWidget_2->setObjectName("DoorDisplayWidget_2");
        DoorDisplayWidget_2->setGeometry(QRect(20, 95, 371, 151));
        DoorDisplayWidget_2->setStyleSheet(QString::fromUtf8("#DoorDisplayWidget_2 {\n"
"	Background-color: rgba(209, 207, 207,50);\n"
"	border-radius: 10px;\n"
"}"));
        TopBanner_2 = new QWidget(Background);
        TopBanner_2->setObjectName("TopBanner_2");
        TopBanner_2->setGeometry(QRect(0, 5, 801, 71));
        TopBanner_2->setAutoFillBackground(false);
        TopBanner_2->setStyleSheet(QString::fromUtf8("#TopBanner_2 {\n"
"	Background-color: rgba(209, 207, 207,50);\n"
"}"));
        btnTrainApproaching->raise();
        btnDoorProximitySensor->raise();
        btnHindranceObstacle->raise();
        btnEmergencyRelease->raise();
        lblTrainIsApproachingState->raise();
        lblDoorProximitySensorState->raise();
        lblHindranceObstacleState->raise();
        lblEmergencyReleaseState->raise();
        btnToggleDebugLED->raise();
        DoorDisplayWidget_2->raise();
        DoorDisplayWidget->raise();
        TopBanner_2->raise();
        TopBanner->raise();
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "PSD Control Panel", nullptr));
        txtDoorState->setText(QCoreApplication::translate("MainWindow", "Door State Display", nullptr));
        lblCurrentState->setText(QCoreApplication::translate("MainWindow", "CLOSED", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "PSD Control Panel", nullptr));
        btnTrainApproaching->setText(QCoreApplication::translate("MainWindow", "TrainApproaching", nullptr));
        btnDoorProximitySensor->setText(QCoreApplication::translate("MainWindow", "DoorProximitySensor", nullptr));
        btnHindranceObstacle->setText(QCoreApplication::translate("MainWindow", "HindranceObstacleDetetction", nullptr));
        btnEmergencyRelease->setText(QCoreApplication::translate("MainWindow", "EmergencyRelease", nullptr));
        lblTrainIsApproachingState->setText(QCoreApplication::translate("MainWindow", "false", nullptr));
        lblDoorProximitySensorState->setText(QCoreApplication::translate("MainWindow", "false", nullptr));
        lblHindranceObstacleState->setText(QCoreApplication::translate("MainWindow", "false", nullptr));
        lblEmergencyReleaseState->setText(QCoreApplication::translate("MainWindow", "false", nullptr));
        btnToggleDebugLED->setText(QCoreApplication::translate("MainWindow", "Toggle Debug LED", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
