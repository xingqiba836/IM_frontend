/******************************************************************************
 *
 * @file       applyfrienditem.h
 * @brief      Single friend apply row
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef APPLYFRIENDITEM_H
#define APPLYFRIENDITEM_H

#include "listitembase.h"
#include "userdata.h"

#include <memory>

namespace Ui {
class ApplyFriendItem;
}

class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem() override;

    void SetInfo(const std::shared_ptr<ApplyInfo> &apply_info);
    void ShowAddBtn(bool bshow);
    QSize sizeHint() const override
    {
        return QSize(250, 80);
    }
    int GetUid() const;

signals:
    void sig_auth_friend(const std::shared_ptr<ApplyInfo> &apply_info);

private:
    Ui::ApplyFriendItem *ui;
    std::shared_ptr<ApplyInfo> _apply_info;
    bool _added = false;
};

#endif // APPLYFRIENDITEM_H
