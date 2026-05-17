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
#include "logindialog.h"
#include "registerdialog.h"
#include "ui_mainwindow.h"

#include <QStackedWidget>

namespace {
void setupEmbeddedPage(QWidget *page)
{
    page->setWindowFlags(Qt::Widget);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    page->setAutoFillBackground(true);
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _stack(nullptr)
    , _login_dlg(nullptr)
    , _reg_dlg(nullptr)
{
    ui->setupUi(this);
    setFixedSize(300, 500);
    setContentsMargins(0, 0, 0, 0);

    _login_dlg = new LoginDialog(this);
    _reg_dlg = new RegisterDialog(this);
    setupEmbeddedPage(_login_dlg);
    setupEmbeddedPage(_reg_dlg);

    _stack = new QStackedWidget(this);
    _stack->setObjectName(QStringLiteral("page_stack"));
    _stack->setContentsMargins(0, 0, 0, 0);
    _stack->addWidget(_login_dlg);
    _stack->addWidget(_reg_dlg);
    setCentralWidget(_stack);

    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    connect(_reg_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    _stack->setCurrentWidget(_reg_dlg);
}

void MainWindow::SlotSwitchLogin()
{
    _stack->setCurrentWidget(_login_dlg);
}
