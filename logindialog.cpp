/******************************************************************************
 *
 * @file       logindialog.cpp
 * @brief      Login dialog for chat client
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "logindialog.h"
#include "clickedlabel.h"
#include "httpmgr.h"
#include "tcpmgr.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

namespace {
void setupWeChatLogo(QLabel *label, int size)
{
    const QPixmap src(QStringLiteral(":/res/wechat.png"));
    if (src.isNull()) {
        return;
    }
    label->setPixmap(src.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setAlignment(Qt::AlignCenter);
}
}

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    setupWeChatLogo(ui->logo_lb, 88);

    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    ui->forget_label->SetState(QStringLiteral("normal"), QStringLiteral("hover"), QString(),
                               QStringLiteral("selected"), QStringLiteral("selected_hover"), QString());
    ui->forget_label->setCursor(Qt::PointingHandCursor);
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);

    initHttpHandlers();
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish,
            this, &LoginDialog::slot_login_mod_finish);

    connect(this, &LoginDialog::sig_connect_tcp,
            TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_con_success,
            this, &LoginDialog::slot_tcp_con_finish);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_failed,
            this, &LoginDialog::slot_login_failed);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::showTip(const QString &text, bool ok)
{
    ui->err_tip->setProperty("state", ok ? "normal" : "err");
    ui->err_tip->setText(text);
    repolish(ui->err_tip);
}

void LoginDialog::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->reg_btn->setEnabled(enabled);
}

bool LoginDialog::checkUserValid()
{
    if (ui->user_edit->text().trimmed().isEmpty()) {
        qDebug() << "User empty";
        showTip(tr("请输入帐号"), false);
        return false;
    }
    return true;
}

bool LoginDialog::checkPwdValid()
{
    const auto pwd = ui->pass_edit->text();
    if (pwd.length() < 6 || pwd.length() > 15) {
        qDebug() << "Pass length invalid";
        showTip(tr("密码长度应为6~15"), false);
        return false;
    }
    return true;
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](const QJsonObject &jsonObj) {
        const int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            qDebug() << "login failed, server error:" << error;
            showTip(serverErrorTip(error), false);
            enableBtn(true);
            return;
        }

        const auto user = jsonObj["user"].toString();
        ServerInfo si;
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();

        _uid = si.Uid;
        _token = si.Token;
        qDebug() << "user is " << user << " uid is " << si.Uid << " host is "
                 << si.Host << " Port is " << si.Port << " Token is " << si.Token;
        showTip(tr("登录成功，正在连接聊天服务..."), true);
        emit sig_connect_tcp(si);
    });
}

void LoginDialog::on_login_btn_clicked()
{
    qDebug() << "login btn clicked";
    if (!checkUserValid()) {
        return;
    }
    if (!checkPwdValid()) {
        return;
    }

    enableBtn(false);

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text().trimmed();
    json_obj["passwd"] = xorString(ui->pass_edit->text());
    qDebug() << "login request to" << gate_url_prefix + QStringLiteral("/user_login");
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + QStringLiteral("/user_login")),
        json_obj,
        ReqId::ID_LOGIN_USER,
        Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        enableBtn(true);
        return;
    }

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"), false);
        enableBtn(true);
        return;
    }

    if (!_handlers.contains(id)) {
        showTip(tr("未知响应类型"), false);
        enableBtn(true);
        return;
    }

    _handlers[id](jsonDoc.object());
}

void LoginDialog::slot_tcp_con_finish(bool bsuccess)
{
    if (bsuccess) {
        showTip(tr("聊天服务连接成功，正在登录..."), true);

        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;

        const QJsonDocument doc(jsonObj);
        const QString jsonString = doc.toJson(QJsonDocument::Indented);

        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    } else {
        showTip(tr("网络异常"), false);
        enableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int err)
{
    const QString result = QStringLiteral("登录失败, err is %1").arg(err);
    showTip(result, false);
    enableBtn(true);
}

void LoginDialog::slot_forget_pwd()
{
    qDebug() << "slot forget pwd";
    emit switchReset();
}
