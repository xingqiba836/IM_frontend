/******************************************************************************
 *
 * @file       friendinfopage.h
 * @brief      Friend profile page in chat dialog
 *
 * @author     hexing
 * @date       2026/08/12
 *
 *****************************************************************************/
#ifndef FRIENDINFOPAGE_H
#define FRIENDINFOPAGE_H

#include "userdata.h"

#include <QWidget>

#include <memory>

namespace Ui {
class FriendInfoPage;
}

class FriendInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit FriendInfoPage(QWidget *parent = nullptr);
    ~FriendInfoPage() override;

    void SetInfo(const std::shared_ptr<UserInfo> &user_info);

signals:
    void sig_jump_chat_item(const std::shared_ptr<UserInfo> &si);

private slots:
    void slot_msg_chat_clicked();

private:
    Ui::FriendInfoPage *ui;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // FRIENDINFOPAGE_H
