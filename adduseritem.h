/******************************************************************************
 *
 * @file       adduseritem.h
 * @brief      Search list tip row for adding users
 *
 * @author     hexing
 * @date       2026/07/08
 *
 *****************************************************************************/
#ifndef ADDUSERITEM_H
#define ADDUSERITEM_H

#include "listitembase.h"

namespace Ui {
class AddUserItem;
}

class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem() override;

    QSize sizeHint() const override
    {
        return QSize(250, 70);
    }

private:
    Ui::AddUserItem *ui;
};

#endif // ADDUSERITEM_H
