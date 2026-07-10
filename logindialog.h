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

#include "global.h"

#include <QDialog>
#include <QJsonObject>
#include <QMap>

#include <functional>

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
    void switchReset();
    void sig_connect_tcp(ServerInfo si);

private slots:
    void on_login_btn_clicked();
    void slot_forget_pwd();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int err);

private:
    void showTip(const QString &text, bool ok);
    void enableBtn(bool enabled);
    void initHttpHandlers();
    bool checkUserValid();
    bool checkPwdValid();

    Ui::LoginDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    int _uid = 0;
    QString _token;
};

#endif // LOGINDIALOG_H
