/******************************************************************************

 *

 * @file       chatdialog.h

 * @brief      Main chat interface

 *

 * @author     hexing

 * @date       2026/05/17

 *

 *****************************************************************************/

#ifndef CHATDIALOG_H

#define CHATDIALOG_H



#include "global.h"
#include "userdata.h"



#include <QDialog>
#include <QList>
#include <QListWidgetItem>
#include <QMap>

class QMouseEvent;
class QTimer;
class StateWidget;
class QWidget;



QT_BEGIN_NAMESPACE

namespace Ui {

class ChatDialog;

}

QT_END_NAMESPACE



class ChatDialog : public QDialog

{

    Q_OBJECT



public:

    explicit ChatDialog(QWidget *parent = nullptr);

    ~ChatDialog() override;



protected:

    bool eventFilter(QObject *watched, QEvent *event) override;



private slots:

    void slot_loading_chat_user();

    void slot_loading_contact_user();

    void slot_side_chat();

    void slot_side_contact();

    void slot_text_changed(const QString &str);

    void slot_switch_apply_friend_page();

    void slot_friend_info_page(const std::shared_ptr<UserInfo> &user_info);

    void slot_show_search(bool show);

    void slot_apply_friend(const std::shared_ptr<AddFriendApply> &apply);

    void slot_add_auth_friend(const std::shared_ptr<AuthInfo> &auth_info);

    void slot_auth_rsp(const std::shared_ptr<AuthRsp> &auth_rsp);

    void slot_jump_chat_item(const std::shared_ptr<SearchInfo> &si);

    void slot_jump_chat_item_from_infopage(const std::shared_ptr<UserInfo> &user_info);

    void slot_item_clicked(QListWidgetItem *item);

    void slot_text_chat_msg(const std::vector<std::shared_ptr<TextChatData>> &msglists);



private:

    void setupSearchEdit();

    void ShowSearch(bool bsearch);

    void addChatUserList();

    void loadFriendChatList();

    void addChatListItem(const std::shared_ptr<UserInfo> &user_info);

    void selectChatByUid(int uid);

    void loadMoreConUser();

    void AddLBGroup(StateWidget *lb);

    void ClearLabelState(StateWidget *lb);

    void handleGlobalMousePress(QMouseEvent *event);

    void UpdateChatMsg(const std::vector<std::shared_ptr<TextChatData>> &msgdata);



    Ui::ChatDialog *ui;

    ChatUIMode _mode;

    ChatUIMode _state;

    bool _b_loading;

    QList<StateWidget *> _lb_list;

    QWidget *_last_widget = nullptr;

    QMap<int, QListWidgetItem *> _chat_items_added;

    int _cur_chat_uid = 0;

    QTimer *_timer = nullptr;
};



#endif // CHATDIALOG_H
