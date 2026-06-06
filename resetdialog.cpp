/******************************************************************************
 *
 * @file       resetdialog.cpp
 * @brief      Reset password dialog for chat client
 *
 * @author     hexing
 * @date       2026/05/25
 *
 *****************************************************************************/
#include "resetdialog.h"
#include "clickedlabel.h"
#include "httpmgr.h"
#include "ui_resetdialog.h"

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
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

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);

    ui->pwd_edit->setEchoMode(QLineEdit::Password);

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();

    setupWeChatLogo(ui->logo_lb, 56);

    connect(ui->user_edit, &QLineEdit::editingFinished, this, [this]() {
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this]() {
        checkEmailValid();
    });
    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this]() {
        checkPassValid();
    });
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this]() {
        checkVarifyValid();
    });

    ui->pwd_visible->setCursor(Qt::PointingHandCursor);
    ui->pwd_visible->SetState(QStringLiteral("unvisible"), QStringLiteral("unvisible_hover"), QString(),
                              QStringLiteral("visible"), QStringLiteral("visible_hover"), QString());
    connect(ui->pwd_visible, &ClickedLabel::clicked, this, [this]() {
        if (ui->pwd_visible->GetCurState() == ClickLbState::Normal) {
            ui->pwd_edit->setEchoMode(QLineEdit::Password);
        } else {
            ui->pwd_edit->setEchoMode(QLineEdit::Normal);
        }
    });

    initHandlers();
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish,
            this, &ResetDialog::slot_reset_mod_finish);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::showTip(const QString &text, bool ok)
{
    ui->err_tip->setProperty("state", ok ? "normal" : "err");
    ui->err_tip->setText(text);
    repolish(ui->err_tip);
}

void ResetDialog::AddTipErr(TipErr te, const QString &tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void ResetDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if (_tip_errs.isEmpty()) {
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

bool ResetDialog::checkUserValid()
{
    if (ui->user_edit->text().isEmpty()) {
        AddTipErr(TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TIP_USER_ERR);
    return true;
}

bool ResetDialog::checkPassValid()
{
    const auto pass = ui->pwd_edit->text();
    if (pass.length() < 6 || pass.length() > 15) {
        AddTipErr(TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    QRegularExpression regExp(QStringLiteral("^[a-zA-Z0-9!@#$%^&*]{6,15}$"));
    if (!regExp.match(pass).hasMatch()) {
        AddTipErr(TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TIP_PWD_ERR);
    return true;
}

bool ResetDialog::checkEmailValid()
{
    const auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    if (!regex.match(email).hasMatch()) {
        AddTipErr(TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TIP_EMAIL_ERR);
    return true;
}

bool ResetDialog::checkVarifyValid()
{
    if (ui->varify_edit->text().isEmpty()) {
        AddTipErr(TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TIP_VARIFY_ERR);
    return true;
}

void ResetDialog::initHandlers()
{
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject &jsonObj) {
        const int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        const auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });

    _handlers.insert(ReqId::ID_RESET_PWD, [this](const QJsonObject &jsonObj) {
        const int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        const auto email = jsonObj["email"].toString();
        showTip(tr("重置成功,点击返回登录"), true);
        qDebug() << "email is " << email;
        if (jsonObj.contains(QStringLiteral("uid"))) {
            qDebug() << "user uid is " << jsonObj["uid"].toInt();
        }
    });
}

void ResetDialog::on_varify_btn_clicked()
{
    if (!checkEmailValid()) {
        return;
    }

    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + QStringLiteral("/get_varifycode")),
        json_obj,
        ReqId::ID_GET_VARIFY_CODE,
        Modules::RESETMOD);
}

void ResetDialog::slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err)
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

void ResetDialog::on_sure_btn_clicked()
{
    if (!checkUserValid()) {
        return;
    }
    if (!checkEmailValid()) {
        return;
    }
    if (!checkPassValid()) {
        return;
    }
    if (!checkVarifyValid()) {
        return;
    }

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pwd_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + QStringLiteral("/reset_pwd")),
        json_obj,
        ReqId::ID_RESET_PWD,
        Modules::RESETMOD);
}

void ResetDialog::on_cancel_btn_clicked()
{
    emit switchLogin();
}
