/******************************************************************************
 *
 * @file       mainwindow.cpp
 * @brief      Main window and page switching
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "mainwindow.h"
#include "chatdialog.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "tcpmgr.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QIcon>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _login_dlg(nullptr)
    , _reg_dlg(nullptr)
    , _reset_dlg(nullptr)
    , _chat_dlg(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(QStringLiteral(":/res/wechat.png")));
    setFixedSize(300, 500);
    setContentsMargins(0, 0, 0, 0);

    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    connectLoginSignals(_login_dlg);

    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_swich_chatdlg,
            this, &MainWindow::SlotSwitchChat);

    // day18: 方便测试，启动后直接进入聊天界面
    QTimer::singleShot(0, this, &MainWindow::SlotSwitchChat);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectLoginSignals(LoginDialog *dlg)
{
    connect(dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchChat()
{
    if (!_chat_dlg) {
        _chat_dlg = new ChatDialog(this);
    }
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();

    if (_login_dlg) {
        _login_dlg->hide();
    }
    if (_reg_dlg) {
        _reg_dlg->hide();
    }
    if (_reset_dlg) {
        _reset_dlg->hide();
    }

    setMinimumSize(QSize(1050, 900));
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    resize(1050, 900);
}

void MainWindow::SlotSwitchReg()
{
    _reg_dlg = new RegisterDialog(this);
    _reg_dlg->hide();
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    setCentralWidget(_reg_dlg);
    if (_login_dlg) {
        _login_dlg->hide();
    }
    _reg_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);

    if (_reg_dlg) {
        _reg_dlg->hide();
    }
    _login_dlg->show();
    connectLoginSignals(_login_dlg);

    setMinimumSize(QSize(300, 500));
    setMaximumSize(QSize(300, 500));
    setFixedSize(300, 500);
}

void MainWindow::SlotSwitchReset()
{
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);

    if (_login_dlg) {
        _login_dlg->hide();
    }
    _reset_dlg->show();

    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
}

void MainWindow::SlotSwitchLogin2()
{
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);

    if (_reset_dlg) {
        _reset_dlg->hide();
    }
    _login_dlg->show();
    connectLoginSignals(_login_dlg);

    setMinimumSize(QSize(300, 500));
    setMaximumSize(QSize(300, 500));
    setFixedSize(300, 500);
}
