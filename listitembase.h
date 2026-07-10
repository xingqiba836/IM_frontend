/******************************************************************************
 *
 * @file       listitembase.h
 * @brief      Base class for list item widgets
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include "global.h"

#include <QWidget>

class ListItemBase : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBase(QWidget *parent = nullptr);

    void SetItemType(ListItemType itemType);
    ListItemType GetItemType() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ListItemType _itemType;
};

#endif // LISTITEMBASE_H
