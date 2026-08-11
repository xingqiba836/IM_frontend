/******************************************************************************
 *
 * @file       chatpage.cpp
 * @brief      Chat message page
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatpage.h"
#include "chatitembase.h"
#include "messagetextedit.h"
#include "picturebubble.h"
#include "textbubble.h"
#include "tcpmgr.h"
#include "ui_chatpage.h"
#include "usermgr.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>
#include <QUuid>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    setAutoFillBackground(true);

    ui->receive_btn->SetState(QStringLiteral("normal"),
                              QStringLiteral("hover"),
                              QStringLiteral("press"));
    ui->send_btn->SetState(QStringLiteral("normal"),
                           QStringLiteral("hover"),
                           QStringLiteral("press"));

    ui->emo_lb->SetState(QStringLiteral("normal"),
                         QStringLiteral("hover"),
                         QStringLiteral("press"),
                         QStringLiteral("normal"),
                         QStringLiteral("hover"),
                         QStringLiteral("press"));
    ui->file_lb->SetState(QStringLiteral("normal"),
                          QStringLiteral("hover"),
                          QStringLiteral("press"),
                          QStringLiteral("normal"),
                          QStringLiteral("hover"),
                          QStringLiteral("press"));

    connect(ui->send_btn, &QPushButton::clicked, this, &ChatPage::on_send_btn_clicked);
    connect(ui->receive_btn, &QPushButton::clicked, this, &ChatPage::on_receive_btn_clicked);
    connect(ui->chatEdit, &MessageTextEdit::send, this, &ChatPage::on_send_btn_clicked);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(const std::shared_ptr<UserInfo> &user_info)
{
    _user_info = user_info;
    if (!user_info) {
        return;
    }

    ui->title_lb->setText(user_info->_name);

    QPixmap headSrc(user_info->_icon);
    if (headSrc.isNull()) {
        headSrc = QPixmap(QStringLiteral(":/res/head_1.jpg"));
    }
    ui->head_lb->setPixmap(headSrc.scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->head_lb->setScaledContents(true);

    ui->chat_data_list->removeAllItem();
}

void ChatPage::AppendChatMsg(const std::shared_ptr<TextChatData> &msg)
{
    if (!msg) {
        return;
    }

    const auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role = ChatRole::Other;
    QString userName;
    QString userIcon;

    if (msg->GetSendUid() == self_info->_uid) {
        role = ChatRole::Self;
        userName = self_info->_name;
        userIcon = self_info->_icon;
    } else {
        const auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->GetSendUid());
        if (!friend_info) {
            return;
        }
        userName = friend_info->_name;
        userIcon = friend_info->_icon;
    }

    auto *pChatItem = new ChatItemBase(role);
    pChatItem->setUserName(userName);
    pChatItem->setUserIcon(QPixmap(userIcon));

    QWidget *pBubble = nullptr;
    if (msg->GetMsgType() == ChatMsgType::TEXT) {
        pBubble = new TextBubble(role, msg->GetMsgContent());
    }

    if (pBubble != nullptr) {
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}

void ChatPage::appendMsgList(const QVector<MsgInfo> &msgList,
                             ChatRole role,
                             const QString &userName,
                             const QString &userIcon)
{
    for (const MsgInfo &msgInfo : msgList) {
        const QString type = msgInfo.msgFlag;
        auto *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));

        QWidget *pBubble = nullptr;
        if (type == QStringLiteral("text")) {
            pBubble = new TextBubble(role, msgInfo.content);
        } else if (type == QStringLiteral("image")) {
            pBubble = new PictureBubble(QPixmap(msgInfo.content), role);
        }

        if (pBubble != nullptr) {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }
}

void ChatPage::on_send_btn_clicked()
{
    if (_user_info == nullptr) {
        qDebug() << "friend_info is empty";
        return;
    }

    const auto user_info = UserMgr::GetInstance()->GetUserInfo();
    auto *pTextEdit = ui->chatEdit;
    const ChatRole role = ChatRole::Self;
    const QString userName = user_info->_name;
    const QString userIcon = user_info->_icon;

    const QVector<MsgInfo> &msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    int thread_id = UserMgr::GetInstance()->GetThreadIdByUid(_user_info->_uid);
    if (thread_id < 0) {
        thread_id = 0;
    }

    for (int i = 0; i < msgList.size(); ++i) {
        if (msgList[i].content.length() > 1024) {
            continue;
        }

        const QString type = msgList[i].msgFlag;
        auto *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        if (type == QStringLiteral("text")) {
            const QUuid uuid = QUuid::createUuid();
            const QString uuidString = uuid.toString();

            pBubble = new TextBubble(role, msgList[i].content);
            if (txt_size + msgList[i].content.length() > 1024) {
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _user_info->_uid;
                textObj["thread_id"] = thread_id;
                textObj["text_array"] = textArray;
                const QJsonDocument doc(textObj);
                const QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            txt_size += msgList[i].content.length();
            QJsonObject obj;
            const QByteArray utf8Message = msgList[i].content.toUtf8();
            const QString content = QString::fromUtf8(utf8Message);
            obj["content"] = content;
            obj["unique_id"] = uuidString;
            textArray.append(obj);

            const auto txt_msg = std::make_shared<TextChatData>(uuidString,
                                                                thread_id,
                                                                ChatFormType::PRIVATE,
                                                                ChatMsgType::TEXT,
                                                                content,
                                                                user_info->_uid,
                                                                0);
            emit sig_append_send_chat_msg(txt_msg);
        } else if (type == QStringLiteral("image")) {
            pBubble = new PictureBubble(QPixmap(msgList[i].content), role);
        }

        if (pBubble != nullptr) {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }

    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->_uid;
    textObj["touid"] = _user_info->_uid;
    textObj["thread_id"] = thread_id;
    const QJsonDocument doc(textObj);
    const QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}

void ChatPage::on_receive_btn_clicked()
{
    if (!_user_info) {
        return;
    }

    const auto friend_info = UserMgr::GetInstance()->GetFriendById(_user_info->_uid);
    if (!friend_info) {
        return;
    }

    const QVector<MsgInfo> &msgList = ui->chatEdit->getMsgList();
    appendMsgList(msgList, ChatRole::Other, friend_info->_name, friend_info->_icon);
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
