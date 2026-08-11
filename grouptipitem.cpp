#include "grouptipitem.h"
#include "ui_grouptipitem.h"

GroupTipItem::GroupTipItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::GroupTipItem)
{
    ui->setupUi(this);
    setObjectName(QStringLiteral("GroupTipItem"));
    SetItemType(ListItemType::GROUP_TIP_ITEM);
}

GroupTipItem::~GroupTipItem()
{
    delete ui;
}

void GroupTipItem::SetGroupTip(const QString &tip)
{
    ui->tip_lb->setText(tip);
}
