#include "conuseritem.h"
#include "ui_conuseritem.h"

#include <QPixmap>

ConUserItem::ConUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70);
}

void ConUserItem::SetInfo(const std::shared_ptr<AuthInfo> &auth_info)
{
    if (!auth_info) {
        return;
    }
    SetInfo(auth_info->_uid, auth_info->_name, QString());
}

void ConUserItem::SetInfo(const std::shared_ptr<AuthRsp> &auth_rsp)
{
    if (!auth_rsp) {
        return;
    }
    SetInfo(auth_rsp->_uid, QString(), QString());
}

void ConUserItem::SetInfo(int uid, const QString &name, const QString &icon)
{
    _info = std::make_shared<UserInfo>(uid, name, icon);

    if (!icon.isEmpty()) {
        QPixmap pixmap(icon);
        ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
    }
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(name);
}

void ConUserItem::ShowRedPoint(bool show)
{
    ui->red_point->setVisible(show);
}

std::shared_ptr<UserInfo> ConUserItem::GetInfo() const
{
    return _info;
}
