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
#include "ui_logindialog.h"

#include <QLabel>
#include <QPixmap>

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

    setupWeChatLogo(ui->logo_lb, 88);

    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_btn_clicked()
{
    if (ui->user_edit->text().trimmed().isEmpty()) {
        ui->err_tip->setText(tr("请输入帐号"));
        return;
    }
    if (ui->pass_edit->text().isEmpty()) {
        ui->err_tip->setText(tr("请输入密码"));
        return;
    }
    ui->err_tip->setStyleSheet(QString());
    ui->err_tip->setText(tr("登录功能将在后续章节接入服务端"));
}
