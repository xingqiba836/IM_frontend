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
