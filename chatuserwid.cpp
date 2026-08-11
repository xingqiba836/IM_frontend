/******************************************************************************
 *
 * @file       chatuserwid.cpp
 * @brief      Chat list item widget
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatuserwid.h"
#include "ui_chatuserwid.h"

#include <QPixmap>

ChatUserWid::ChatUserWid(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(CHAT_USER_ITEM);
    if (ui->red_point) {
        ui->red_point->raise();
        ShowRedPoint(false);
    }
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

QSize ChatUserWid::sizeHint() const
{
    return QSize(250, 70);
}

void ChatUserWid::SetInfo(const QString &name, const QString &head, const QString &msg)
{
    _name = name;
    _head = head;
    _msg = msg;
    _user_info = std::make_shared<UserInfo>(0, name, head);

    QPixmap pixmap(_head);
    if (pixmap.isNull()) {
        pixmap = QPixmap(QStringLiteral(":/res/wechat.png"));
    }

    const QSize iconSize = ui->icon_lb->minimumSize().isValid()
                               ? ui->icon_lb->minimumSize()
                               : QSize(44, 44);
    ui->icon_lb->setPixmap(pixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_name);
    ui->user_chat_lb->setText(_msg);
    ui->time_lb->setText(QStringLiteral("12:00"));
}

void ChatUserWid::SetInfo(const std::shared_ptr<UserInfo> &user_info)
{
    if (!user_info) {
        return;
    }

    _user_info = user_info;
    _name = user_info->_name;
    _head = user_info->_icon;
    _msg = QString();

    QPixmap pixmap(_head);
    if (pixmap.isNull()) {
        pixmap = QPixmap(QStringLiteral(":/res/head_1.jpg"));
    }

    const QSize iconSize = ui->icon_lb->minimumSize().isValid()
                               ? ui->icon_lb->minimumSize()
                               : QSize(44, 44);
    ui->icon_lb->setPixmap(pixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(user_info->_name);
    ui->user_chat_lb->clear();
    ui->time_lb->setText(QStringLiteral("12:00"));
}

std::shared_ptr<UserInfo> ChatUserWid::GetUserInfo() const
{
    return _user_info;
}

void ChatUserWid::ShowRedPoint(bool bshow)
{
    if (!ui->red_point) {
        return;
    }

    if (bshow) {
        ui->red_point->show();
    } else {
        ui->red_point->hide();
    }
}

void ChatUserWid::updateLastMsg(const std::vector<std::shared_ptr<TextChatData>> &msgs)
{
    QString last_msg;
    for (const auto &msg : msgs) {
        if (msg) {
            last_msg = msg->GetContent();
        }
    }

    if (!last_msg.isEmpty()) {
        ui->user_chat_lb->setText(last_msg);
    }
}
