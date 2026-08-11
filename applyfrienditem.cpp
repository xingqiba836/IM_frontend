#include "applyfrienditem.h"
#include "clickedbtn.h"
#include "ui_applyfrienditem.h"

#include <QPixmap>

ApplyFriendItem::ApplyFriendItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ApplyFriendItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    ui->addBtn->SetState(QStringLiteral("normal"),
                         QStringLiteral("hover"),
                         QStringLiteral("press"));
    ui->addBtn->hide();
    connect(ui->addBtn, &ClickedBtn::clicked, this, [this]() {
        emit sig_auth_friend(_apply_info);
    });
}

ApplyFriendItem::~ApplyFriendItem()
{
    delete ui;
}

void ApplyFriendItem::SetInfo(const std::shared_ptr<ApplyInfo> &apply_info)
{
    _apply_info = apply_info;
    if (!apply_info) {
        return;
    }

    QPixmap pixmap(apply_info->_icon);
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(apply_info->_name);
    ui->user_chat_lb->setText(apply_info->_desc);
}

void ApplyFriendItem::ShowAddBtn(bool bshow)
{
    if (bshow) {
        ui->addBtn->show();
        ui->already_add_lb->hide();
        _added = false;
    } else {
        ui->addBtn->hide();
        ui->already_add_lb->show();
        _added = true;
    }
}

int ApplyFriendItem::GetUid() const
{
    return _apply_info ? _apply_info->_uid : 0;
}
