/******************************************************************************
 *
 * @file       grouptipitem.h
 * @brief      Group header row in contact list
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H

#include "listitembase.h"

namespace Ui {
class GroupTipItem;
}

class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem() override;

    void SetGroupTip(const QString &tip);

    QSize sizeHint() const override
    {
        return QSize(250, 30);
    }

private:
    Ui::GroupTipItem *ui;
};

#endif // GROUPTIPITEM_H
