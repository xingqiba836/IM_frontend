/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      Main window and page switching
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class LoginDialog;
class RegisterDialog;
class QStackedWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void SlotSwitchReg();
    void SlotSwitchLogin();

private:
    Ui::MainWindow *ui;
    QStackedWidget *_stack;
    LoginDialog *_login_dlg;
    RegisterDialog *_reg_dlg;
};

#endif // MAINWINDOW_H
