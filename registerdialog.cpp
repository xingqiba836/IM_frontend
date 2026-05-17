/******************************************************************************
 *
 * @file       registerdialog.cpp
 * @brief      Register dialog for chat client
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "registerdialog.h"
#include "httpmgr.h"
#include "ui_registerdialog.h"

#include <QLabel>
#include <QPixmap>
#include <QRegularExpression>
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

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);

    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);

    setupWeChatLogo(ui->logo_lb, 56);
    initHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterDialog::slot_reg_mod_finish);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::showTip(const QString &text, bool ok)
{
    ui->err_tip->setProperty("state", ok ? "normal" : "err");
    ui->err_tip->setText(text);
    repolish(ui->err_tip);
}

void RegisterDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject &jsonObj) {
        const int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            if (error == ErrorCodes::ERR_JSON) {
                showTip(tr("json解析错误"), false);
            } else {
                showTip(tr("参数错误"), false);
            }
            return;
        }
        const auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });
}

void RegisterDialog::on_varify_btn_clicked()
{
    const auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    const bool match = regex.match(email).hasMatch();
    if (match) {
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(
            QUrl(gate_url_prefix + QStringLiteral("/get_varifycode")),
            json_obj,
            ReqId::ID_GET_VARIFY_CODE,
            Modules::REGISTERMOD);
    } else {
        showTip(tr("邮箱地址不正确"), false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return;
    }

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"), false);
        return;
    }

    if (!_handlers.contains(id)) {
        showTip(tr("未知响应类型"), false);
        return;
    }

    _handlers[id](jsonDoc.object());
}

void RegisterDialog::on_sure_btn_clicked()
{
    if (ui->user_edit->text().trimmed().isEmpty()) {
        showTip(tr("用户名不能为空"), false);
        return;
    }
    if (ui->email_edit->text().trimmed().isEmpty()) {
        showTip(tr("邮箱不能为空"), false);
        return;
    }
    QRegularExpression emailRegex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    if (!emailRegex.match(ui->email_edit->text()).hasMatch()) {
        showTip(tr("邮箱地址不正确"), false);
        return;
    }
    const auto pass = ui->pass_edit->text();
    if (pass.length() < 6 || pass.length() > 15) {
        showTip(tr("密码长度应为6~15"), false);
        return;
    }
    if (ui->confirm_edit->text() != pass) {
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }
    if (ui->varify_edit->text().trimmed().isEmpty()) {
        showTip(tr("验证码不能为空"), false);
        return;
    }
    showTip(tr("注册功能将在后续章节接入服务端"), true);
}

void RegisterDialog::on_cancel_btn_clicked()
{
    emit sigSwitchLogin();
}
