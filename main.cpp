/******************************************************************************
 *
 * @file       main.cpp
 * @brief      Application entry
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "mainwindow.h"
#include "global.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QStringLiteral(":/res/wechat.png")));

    const QString app_path = QCoreApplication::applicationDirPath();
    const QString fileName = QStringLiteral("config.ini");
    const QString config_path = QDir::toNativeSeparators(
        app_path + QDir::separator() + fileName);

    QSettings settings(config_path, QSettings::IniFormat);
    const QString gate_host = settings.value(QStringLiteral("GateServer/host")).toString();
    const QString gate_port = settings.value(QStringLiteral("GateServer/port")).toString();
    gate_url_prefix = QStringLiteral("http://") + gate_host + QStringLiteral(":") + gate_port;

    QFile qss(":/style/stylesheet.qss");
    if (qss.open(QFile::ReadOnly)) {
        const QString style = QString::fromUtf8(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    } else {
        qDebug("Open stylesheet failed");
    }

    MainWindow w;
    w.show();

    return a.exec();
}
