/******************************************************************************
 *
 * @file       chatdialog.cpp
 * @brief      Main chat interface
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatdialog.h"
#include "applyfriend.h"
#include "applyfriendpage.h"
#include "chatpage.h"
#include "chatuserlist.h"
#include "chatuserwid.h"
#include "conuseritem.h"
#include "contactuserlist.h"
#include "friendinfopage.h"
#include "loadingdlg.h"
#include "searchlist.h"
#include "statewidget.h"
#include "tcpmgr.h"
#include "ui_chatdialog.h"
#include "usermgr.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QPixmap>
#include <QRandomGenerator>
#include <QTimer>
#include <QWidget>

namespace {
const QStringList kMsgs = {
    QStringLiteral("hello world !"),
    QStringLiteral("nice to meet u"),
    QStringLiteral("New year, new life"),
    QStringLiteral("You have to love yourself"),
    QStringLiteral("My love is written in the wind"),
};

const QStringList kHeads = {
    QStringLiteral(":/res/head_1.jpg"),
    QStringLiteral(":/res/head_2.jpg"),
    QStringLiteral(":/res/head_3.jpg"),
    QStringLiteral(":/res/head_4.jpg"),
    QStringLiteral(":/res/head_5.jpg"),
};

const QStringList kNames = {
    QStringLiteral("hexing"),
    QStringLiteral("zack"),
    QStringLiteral("golang"),
    QStringLiteral("cpp"),
    QStringLiteral("java"),
    QStringLiteral("nodejs"),
    QStringLiteral("python"),
    QStringLiteral("rust"),
};
}

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
    , _mode(ChatUIMode::ChatMode)
    , _state(ChatUIMode::ChatMode)
    , _b_loading(false)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(QStringLiteral(":/res/wechat.png")));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);

    ui->add_btn->SetState(QStringLiteral("normal"),
                          QStringLiteral("hover"),
                          QStringLiteral("press"));

    setupSearchEdit();

    const auto self_info = UserMgr::GetInstance()->GetUserInfo();
    const QString head_path = self_info && !self_info->_icon.isEmpty()
                                  ? self_info->_icon
                                  : QStringLiteral(":/res/head_1.jpg");
    const QPixmap pixmap(head_path);
    const QPixmap scaledPixmap = pixmap.scaled(ui->side_head_lb->size(),
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);
    ui->side_head_lb->setPixmap(scaledPixmap);
    ui->side_head_lb->setScaledContents(true);

    ui->side_chat_lb->setProperty("state", QStringLiteral("normal"));
    ui->side_chat_lb->SetState(QStringLiteral("normal"),
                               QStringLiteral("hover"),
                               QStringLiteral("pressed"),
                               QStringLiteral("selected_normal"),
                               QStringLiteral("selected_hover"),
                               QStringLiteral("selected_pressed"));
    ui->side_contact_lb->SetState(QStringLiteral("normal"),
                                  QStringLiteral("hover"),
                                  QStringLiteral("pressed"),
                                  QStringLiteral("selected_normal"),
                                  QStringLiteral("selected_hover"),
                                  QStringLiteral("selected_pressed"));

    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);

    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    ui->search_list->SetSearchEdit(ui->search_edit);

    ShowSearch(false);
    loadFriendChatList();
    if (_chat_items_added.isEmpty()) {
        addChatUserList();
    }

    ui->side_chat_lb->SetSelected(true);
    ui->right_stack->setCurrentWidget(ui->chat_page);

    qApp->installEventFilter(this);
    ui->search_edit->installEventFilter(this);

    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user,
            this, &ChatDialog::slot_loading_chat_user);
    connect(ui->con_user_list, &ContactUserList::sig_loading_contact_user,
            this, &ChatDialog::slot_loading_contact_user);
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this, &ChatDialog::slot_switch_apply_friend_page);
    connect(ui->con_user_list, &ContactUserList::sig_switch_friend_info_page,
            this, &ChatDialog::slot_friend_info_page);
    connect(ui->friend_apply_page, &ApplyFriendPage::sig_show_search,
            this, &ChatDialog::slot_show_search);

    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply, this, &ChatDialog::slot_apply_friend);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ChatDialog::slot_add_auth_friend);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ChatDialog::slot_auth_rsp);
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item,
            this, &ChatDialog::slot_jump_chat_item_from_infopage);
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_text_chat_msg, this, &ChatDialog::slot_text_chat_msg);
    connect(ui->chat_page, &ChatPage::sig_append_send_chat_msg, this,
            [this](const std::shared_ptr<TextChatData> &msg) {
                if (!msg || _cur_chat_uid == 0) {
                    return;
                }
                const auto find_iter = _chat_items_added.find(_cur_chat_uid);
                if (find_iter == _chat_items_added.end()) {
                    return;
                }
                QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
                if (auto *chat_wid = qobject_cast<ChatUserWid *>(widget)) {
                    chat_wid->updateLastMsg({msg});
                }
            });

    // 定时发送心跳，保持与 ChatServer 的长连接活跃
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [this]() {
        auto user_info = UserMgr::GetInstance()->GetUserInfo();
        int uid = 0;
        if (user_info) {
            uid = user_info->_uid;
        } else {
            uid = UserMgr::GetInstance()->GetUid();
        }
        if (uid <= 0) {
            return;
        }

        QJsonObject textObj;
        textObj["fromuid"] = uid;
        const QJsonDocument doc(textObj);
        const QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_HEART_BEAT_REQ, jsonData);
    });
    _timer->start(10000);
}

ChatDialog::~ChatDialog()
{
    if (_timer) {
        _timer->stop();
    }
    qApp->removeEventFilter(this);
    delete ui;
}

void ChatDialog::setupSearchEdit()
{
    auto *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(QStringLiteral(":/res/search.png")));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    auto *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(QStringLiteral(":/res/close_transparent.png")));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(QStringLiteral(":/res/close_search.png")));
        } else {
            clearAction->setIcon(QIcon(QStringLiteral(":/res/close_transparent.png")));
        }
    });

    connect(clearAction, &QAction::triggered, this, [this, clearAction]() {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(QStringLiteral(":/res/close_transparent.png")));
        ui->search_edit->clearFocus();
        ShowSearch(false);
    });

    ui->search_edit->SetMaxLength(15);
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if (bsearch) {
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    } else if (_state == ChatUIMode::ChatMode) {
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        ui->search_list->CloseFindDlg();
        _mode = ChatUIMode::ChatMode;
    } else if (_state == ChatUIMode::ContactMode) {
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        ui->search_list->CloseFindDlg();
        _mode = ChatUIMode::ContactMode;
    }
}

void ChatDialog::addChatUserList()
{
    for (int i = 0; i < 13; ++i) {
        const int randomValue = QRandomGenerator::global()->bounded(100);
        const int str_i = randomValue % kMsgs.size();
        const int head_i = randomValue % kHeads.size();
        const int name_i = randomValue % kNames.size();

        auto *chat_user_wid = new ChatUserWid();
        chat_user_wid->SetInfo(kNames.at(name_i), kHeads.at(head_i), kMsgs.at(str_i));

        auto *item = new QListWidgetItem;
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::loadFriendChatList()
{
    const auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    for (const auto &friend_info : friend_list) {
        addChatListItem(friend_info);
    }
    UserMgr::GetInstance()->UpdateChatLoadedCount();
}

void ChatDialog::addChatListItem(const std::shared_ptr<UserInfo> &user_info)
{
    if (!user_info || _chat_items_added.contains(user_info->_uid)) {
        return;
    }

    auto *chat_user_wid = new ChatUserWid();
    chat_user_wid->SetInfo(user_info);
    auto *item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(user_info->_uid, item);
}

void ChatDialog::selectChatByUid(int uid)
{
    const auto find_iter = _chat_items_added.find(uid);
    if (find_iter == _chat_items_added.end()) {
        const auto friend_info = UserMgr::GetInstance()->GetFriendById(uid);
        if (!friend_info) {
            return;
        }
        addChatListItem(friend_info);
    }

    const auto item = _chat_items_added.value(uid, nullptr);
    if (!item) {
        return;
    }

    ui->chat_user_list->scrollToItem(item);
    ui->chat_user_list->setCurrentItem(item);

    auto *widget = ui->chat_user_list->itemWidget(item);
    auto *chat_wid = qobject_cast<ChatUserWid *>(widget);
    if (!chat_wid) {
        return;
    }

    const auto user_info = chat_wid->GetUserInfo();
    ui->chat_page->SetUserInfo(user_info);
    _cur_chat_uid = uid;
    ui->right_stack->setCurrentWidget(ui->chat_page);
    ClearLabelState(ui->side_chat_lb);
    ui->side_chat_lb->SetSelected(true);
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::AddLBGroup(StateWidget *lb)
{
    _lb_list.push_back(lb);
}

void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for (auto *ele : _lb_list) {
        if (ele == lb) {
            continue;
        }
        ele->ClearState();
    }
}

void ChatDialog::loadMoreConUser()
{
    const auto con_list = UserMgr::GetInstance()->GetConListPerPage();
    for (const auto &con_ele : con_list) {
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(con_ele->_uid, con_ele->_name, con_ele->_icon);
        auto *item = new QListWidgetItem;
        item->setSizeHint(con_user_wid->sizeHint());
        ui->con_user_list->addItem(item);
        ui->con_user_list->setItemWidget(item, con_user_wid);
    }
    UserMgr::GetInstance()->UpdateContactLoadedCount();
}

void ChatDialog::slot_loading_contact_user()
{
    if (_b_loading) {
        return;
    }

    _b_loading = true;
    auto *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    loadMoreConUser();
    loadingDialog->deleteLater();
    _b_loading = false;
}

void ChatDialog::slot_switch_apply_friend_page()
{
    _last_widget = ui->friend_apply_page;
    ui->right_stack->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_friend_info_page(const std::shared_ptr<UserInfo> &user_info)
{
    _last_widget = ui->friend_info_page;
    ui->right_stack->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

void ChatDialog::slot_show_search(bool show)
{
    ShowSearch(show);
}

void ChatDialog::slot_apply_friend(const std::shared_ptr<AddFriendApply> &apply)
{
    if (!apply) {
        return;
    }

    if (UserMgr::GetInstance()->AlreadyApply(apply->_from_uid)) {
        return;
    }

    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_user_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_add_auth_friend(const std::shared_ptr<AuthInfo> &auth_info)
{
    if (!auth_info) {
        return;
    }

    if (UserMgr::GetInstance()->CheckFriendById(auth_info->_uid)) {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_info);

    const auto user_info = std::make_shared<UserInfo>(auth_info);
    addChatListItem(user_info);

    if (!auth_info->_chat_datas.empty()) {
        const auto item = _chat_items_added.value(auth_info->_uid, nullptr);
        if (item) {
            auto *widget = ui->chat_user_list->itemWidget(item);
            if (auto *chat_wid = qobject_cast<ChatUserWid *>(widget)) {
                chat_wid->updateLastMsg(auth_info->_chat_datas);
            }
        }
    }
}

void ChatDialog::slot_auth_rsp(const std::shared_ptr<AuthRsp> &auth_rsp)
{
    if (!auth_rsp) {
        return;
    }

    if (UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid)) {
        return;
    }

    UserMgr::GetInstance()->AddFriend(auth_rsp);

    const auto user_info = std::make_shared<UserInfo>(auth_rsp);
    addChatListItem(user_info);

    if (!auth_rsp->_chat_datas.empty()) {
        const auto item = _chat_items_added.value(auth_rsp->_uid, nullptr);
        if (item) {
            auto *widget = ui->chat_user_list->itemWidget(item);
            if (auto *chat_wid = qobject_cast<ChatUserWid *>(widget)) {
                chat_wid->updateLastMsg(auth_rsp->_chat_datas);
            }
        }
    }
}

void ChatDialog::slot_jump_chat_item(const std::shared_ptr<SearchInfo> &si)
{
    if (!si) {
        return;
    }

    ui->search_list->CloseFindDlg();
    ui->search_edit->clear();
    ShowSearch(false);

    if (!_chat_items_added.contains(si->_uid)) {
        const auto user_info = std::make_shared<UserInfo>(si);
        addChatListItem(user_info);
    }

    selectChatByUid(si->_uid);
}

void ChatDialog::slot_jump_chat_item_from_infopage(const std::shared_ptr<UserInfo> &user_info)
{
    if (!user_info) {
        return;
    }

    if (!_chat_items_added.contains(user_info->_uid)) {
        addChatListItem(user_info);
    }

    selectChatByUid(user_info->_uid);
}

void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item);
    if (!widget) {
        return;
    }

    auto *customItem = qobject_cast<ListItemBase *>(widget);
    if (!customItem) {
        return;
    }

    if (customItem->GetItemType() != ListItemType::CHAT_USER_ITEM) {
        return;
    }

    auto *chat_wid = qobject_cast<ChatUserWid *>(customItem);
    if (!chat_wid) {
        return;
    }

    const auto user_info = chat_wid->GetUserInfo();
    if (!user_info) {
        return;
    }

    ui->chat_page->SetUserInfo(user_info);
    _cur_chat_uid = user_info->_uid;
    ui->right_stack->setCurrentWidget(ui->chat_page);
}

void ChatDialog::UpdateChatMsg(const std::vector<std::shared_ptr<TextChatData>> &msgdata)
{
    for (const auto &msg : msgdata) {
        if (!msg) {
            continue;
        }
        ui->chat_page->AppendChatMsg(msg);
    }
}

void ChatDialog::slot_text_chat_msg(const std::vector<std::shared_ptr<TextChatData>> &msglists)
{
    for (const auto &msg : msglists) {
        if (!msg) {
            continue;
        }

        const int from_uid = msg->GetSendUid();
        const auto find_iter = _chat_items_added.find(from_uid);
        if (find_iter != _chat_items_added.end()) {
            QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
            if (auto *chat_wid = qobject_cast<ChatUserWid *>(widget)) {
                chat_wid->updateLastMsg({msg});
            }

            if (_cur_chat_uid == from_uid) {
                ui->chat_page->AppendChatMsg(msg);
            }
            continue;
        }

        auto fi_ptr = UserMgr::GetInstance()->GetFriendById(from_uid);
        if (!fi_ptr) {
            continue;
        }

        addChatListItem(fi_ptr);
        const auto item = _chat_items_added.value(from_uid, nullptr);
        if (item) {
            if (auto *chat_wid = qobject_cast<ChatUserWid *>(ui->chat_user_list->itemWidget(item))) {
                chat_wid->updateLastMsg({msg});
            }
        }

        if (_cur_chat_uid == from_uid) {
            ui->chat_page->AppendChatMsg(msg);
        }
    }
}

void ChatDialog::slot_loading_chat_user()
{
    if (_b_loading) {
        return;
    }

    _b_loading = true;
    auto *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();

    const auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    for (const auto &friend_info : friend_list) {
        addChatListItem(friend_info);
    }
    UserMgr::GetInstance()->UpdateChatLoadedCount();

    if (_chat_items_added.isEmpty()) {
        addChatUserList();
    }

    loadingDialog->deleteLater();
    _b_loading = false;
}

void ChatDialog::slot_side_chat()
{
    ClearLabelState(ui->side_chat_lb);
    ui->right_stack->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact()
{
    ClearLabelState(ui->side_contact_lb);
    _last_widget = ui->friend_apply_page;
    ui->right_stack->setCurrentWidget(ui->friend_apply_page);
    _state = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    if (!str.isEmpty()) {
        ShowSearch(true);
    }
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->search_edit && event->type() == QEvent::FocusIn) {
        ShowSearch(true);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        handleGlobalMousePress(mouseEvent);
    }

    return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (qobject_cast<ApplyFriend *>(widget) && widget->isVisible()) {
            return;
        }
    }

    const QPoint globalPos = event->globalPosition().toPoint();

    if (ui->search_list->IsClickOnFindDlg(globalPos)) {
        return;
    }

    QWidget *clickedWidget = QApplication::widgetAt(globalPos);
    for (QWidget *widget = clickedWidget; widget; widget = widget->parentWidget()) {
        if (qobject_cast<ApplyFriend *>(widget)) {
            return;
        }
    }

    auto isClickInWidget = [&](QWidget *widget) {
        if (!widget || !widget->isVisible()) {
            return false;
        }
        if (clickedWidget && (clickedWidget == widget || widget->isAncestorOf(clickedWidget))) {
            return true;
        }
        const QPoint localPos = widget->mapFromGlobal(globalPos);
        return widget->rect().contains(localPos);
    };

    if (isClickInWidget(ui->search_wid)) {
        ui->search_list->CloseFindDlg();
        ShowSearch(true);
        return;
    }

    if (_mode != ChatUIMode::SearchMode) {
        ui->search_list->CloseFindDlg();
        return;
    }

    if (isClickInWidget(ui->search_list)) {
        return;
    }

    ui->search_list->CloseFindDlg();
    ui->search_edit->clear();
    ShowSearch(false);
}
