/******************************************************************************
 *
 * @file       logindialog.h
 * @brief      Login dialog for chat client
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginDialog;
}
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;

signals:
    void switchRegister();

private slots:
    void on_login_btn_clicked();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
