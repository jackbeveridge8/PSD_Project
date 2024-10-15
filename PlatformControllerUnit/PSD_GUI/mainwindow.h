#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnTrainApproaching_clicked();
    void on_btnDoorProximitySensor_clicked();
    void on_btnHindranceObstacle_clicked();
    void on_btnEmergencyRelease_clicked();
    void handleNetworkReply(QNetworkReply *reply);
    void updateState();
    void updateCircleLightStatus();
    void on_btnToggleDebugLED_clicked();
    void updateInputStates();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QTimer *timer;
    QString esp32_ip;

    // Input states
    bool trainApproaching;
    bool doorProximitySensor;
    bool hindranceObstacle;
    bool emergencyRelease;
    bool debugLedStatus;

    void requestDebugLEDState();
    void parseInputStatesJSON(const QString& jsonString);
    void applyShadowEffect();
};

#endif // MAINWINDOW_H
