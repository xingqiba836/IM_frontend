/******************************************************************************
 *
 * @file       listitembase.cpp
 * @brief      Base class for list item widgets
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "listitembase.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOption>

ListItemBase::ListItemBase(QWidget *parent)
    : QWidget(parent)
    , _itemType(CHAT_USER_ITEM)
{
}

void ListItemBase::SetItemType(ListItemType itemType)
{
    _itemType = itemType;
}

ListItemType ListItemBase::GetItemType() const
{
    return _itemType;
}

void ListItemBase::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
