/******************************************************************************
 *
 * @file       conuseritem.h
 * @brief      Contact list item widget
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include "listitembase.h"
#include "userdata.h"

#include <memory>

namespace Ui {
class ConUserItem;
}

class ConUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem() override;

    QSize sizeHint() const override;
    void SetInfo(const std::shared_ptr<AuthInfo> &auth_info);
    void SetInfo(const std::shared_ptr<AuthRsp> &auth_rsp);
    void SetInfo(int uid, const QString &name, const QString &icon);
    void ShowRedPoint(bool show = false);
    std::shared_ptr<UserInfo> GetInfo() const;

private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _info;
};

#endif // CONUSERITEM_H
