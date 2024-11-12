#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir::setCurrent(QCoreApplication::applicationDirPath());  // Ensure working directory is set to executable location
    qDebug() << "Current working directory:" << QDir::currentPath();

    MainWindow w;
    w.show();
    return a.exec();
}
