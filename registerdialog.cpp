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
#include "clickedlabel.h"
#include "httpmgr.h"
#include "ui_registerdialog.h"

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

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    , _countdown_timer(nullptr)
    , _countdown(5)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);

    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();

    setupWeChatLogo(ui->logo_lb, 56);
    initHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,
            this, &RegisterDialog::slot_reg_mod_finish);

    connect(ui->user_edit, &QLineEdit::editingFinished, this, [this]() {
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this]() {
        checkEmailValid();
    });
    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this]() {
        checkPassValid();
    });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this]() {
        checkConfirmValid();
    });
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this]() {
        checkVarifyValid();
    });

    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);
    ui->pass_visible->SetState(QStringLiteral("unvisible"), QStringLiteral("unvisible_hover"), QString(),
                               QStringLiteral("visible"), QStringLiteral("visible_hover"), QString());
    ui->confirm_visible->SetState(QStringLiteral("unvisible"), QStringLiteral("unvisible_hover"), QString(),
                                  QStringLiteral("visible"), QStringLiteral("visible_hover"), QString());

    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        if (ui->pass_visible->GetCurState() == ClickLbState::Normal) {
            ui->pass_edit->setEchoMode(QLineEdit::Password);
        } else {
            ui->pass_edit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        if (ui->confirm_visible->GetCurState() == ClickLbState::Normal) {
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        } else {
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
    });

    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, this, [this]() {
        if (_countdown == 0) {
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        --_countdown;
        ui->tip_lb->setText(tr("注册成功，%1 s后返回登录").arg(_countdown));
    });
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

void RegisterDialog::AddTipErr(TipErr te, const QString &tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if (_tip_errs.isEmpty()) {
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if (ui->user_edit->text().isEmpty()) {
        AddTipErr(TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    const auto pass = ui->pass_edit->text();
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

bool RegisterDialog::checkEmailValid()
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

bool RegisterDialog::checkConfirmValid()
{
    if (ui->confirm_edit->text().isEmpty()) {
        AddTipErr(TIP_CONFIRM_ERR, tr("确认密码不能为空"));
        return false;
    }
    if (ui->confirm_edit->text() != ui->pass_edit->text()) {
        AddTipErr(TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    }
    DelTipErr(TIP_CONFIRM_ERR);
    DelTipErr(TIP_PWD_CONFIRM);
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    if (ui->varify_edit->text().isEmpty()) {
        AddTipErr(TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TIP_VARIFY_ERR);
    return true;
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

    _handlers.insert(ReqId::ID_REG_USER, [this](const QJsonObject &jsonObj) {
        const int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        const auto email = jsonObj["email"].toString();
        qDebug() << "email is " << email;
        if (jsonObj.contains(QStringLiteral("uid"))) {
            qDebug() << "user uid is " << jsonObj["uid"].toInt();
        }
        ChangeTipPage();
    });
}

void RegisterDialog::ChangeTipPage()
{
    _countdown = 5;
    ui->tip_lb->setText(tr("注册成功，%1 s后返回登录").arg(_countdown));
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    _countdown_timer->start(1000);
}

void RegisterDialog::on_varify_btn_clicked()
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
        Modules::REGISTERMOD);
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
    if (!checkUserValid()) {
        return;
    }
    if (!checkEmailValid()) {
        return;
    }
    if (!checkPassValid()) {
        return;
    }
    if (!checkConfirmValid()) {
        return;
    }
    if (!checkVarifyValid()) {
        return;
    }

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->pass_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["varifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(
        QUrl(gate_url_prefix + QStringLiteral("/user_register")),
        json_obj,
        ReqId::ID_REG_USER,
        Modules::REGISTERMOD);
}

void RegisterDialog::on_cancel_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

void RegisterDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}
