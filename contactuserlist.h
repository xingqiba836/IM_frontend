/******************************************************************************
 *
 * @file       contactuserlist.h
 * @brief      Contact list with hover scrollbar
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include "userdata.h"

#include <QListWidget>

#include <memory>

class ConUserItem;
class QListWidgetItem;

class ContactUserList : public QListWidget
{
    Q_OBJECT

public:
    explicit ContactUserList(QWidget *parent = nullptr);

    void ShowRedPoint(bool bshow = true);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void addContactUserList();

public slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_add_auth_firend(const std::shared_ptr<AuthInfo> &auth_info);
    void slot_auth_rsp(const std::shared_ptr<AuthRsp> &auth_rsp);

signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(const std::shared_ptr<UserInfo> &user_info);

private:
    bool _load_pending = false;
    ConUserItem *_add_friend_item = nullptr;
    QListWidgetItem *_groupitem = nullptr;
};

#endif // CONTACTUSERLIST_H
