#include "friendinfopage.h"
#include "clickedbtn.h"
#include "ui_friendinfopage.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPixmap>

FriendInfoPage::FriendInfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendInfoPage)
{
    ui->setupUi(this);
    ui->msg_chat->SetState(QStringLiteral("normal"),
                           QStringLiteral("hover"),
                           QStringLiteral("press"));
    ui->video_chat->SetState(QStringLiteral("normal"),
                             QStringLiteral("hover"),
                             QStringLiteral("press"));
    ui->voice_chat->SetState(QStringLiteral("normal"),
                             QStringLiteral("hover"),
                             QStringLiteral("press"));
    connect(ui->msg_chat, &ClickedBtn::clicked, this, &FriendInfoPage::slot_msg_chat_clicked);
}

FriendInfoPage::~FriendInfoPage()
{
    delete ui;
}

void FriendInfoPage::SetInfo(const std::shared_ptr<UserInfo> &user_info)
{
    _user_info = user_info;
    if (!user_info) {
        return;
    }

    QString pix_path = user_info->_icon;
    if (pix_path.isEmpty() || !QFile::exists(pix_path)) {
        pix_path = QDir::toNativeSeparators(
            QCoreApplication::applicationDirPath()
            + QDir::separator() + QStringLiteral("static")
            + QDir::separator() + QStringLiteral("head_1.jpg"));
        if (!QFile::exists(pix_path)) {
            pix_path = QStringLiteral(":/res/head_1.jpg");
        }
    }

    QPixmap pixmap(pix_path);
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->name_lb->setText(user_info->_name);
    ui->nick_lb->setText(user_info->_name);
    ui->bak_lb->setText(user_info->_name);
}

void FriendInfoPage::slot_msg_chat_clicked()
{
    qDebug() << "msg chat btn clicked";
    if (_user_info) {
        emit sig_jump_chat_item(_user_info);
    }
}
