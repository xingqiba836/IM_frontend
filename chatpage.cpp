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
#include "ui_chatpage.h"

#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>

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

    ui->title_lb->setText(tr("hexing"));

    const QPixmap headSrc(QStringLiteral(":/res/wechat.png"));
    if (!headSrc.isNull()) {
        ui->head_lb->setPixmap(headSrc.scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->head_lb->setScaledContents(true);
    }

    connect(ui->send_btn, &QPushButton::clicked, this, &ChatPage::on_send_btn_clicked);
    connect(ui->receive_btn, &QPushButton::clicked, this, &ChatPage::on_receive_btn_clicked);
    connect(ui->chatEdit, &MessageTextEdit::send, this, &ChatPage::on_send_btn_clicked);
}

ChatPage::~ChatPage()
{
    delete ui;
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
    const ChatRole role = ChatRole::Self;
    const QString userName = QStringLiteral("hexing");
    const QString userIcon = QStringLiteral(":/res/head_1.jpg");
    const QVector<MsgInfo> msgList = ui->chatEdit->getMsgList();
    appendMsgList(msgList, role, userName, userIcon);
}

void ChatPage::on_receive_btn_clicked()
{
    const ChatRole role = ChatRole::Other;
    const QString userName = QStringLiteral("zack");
    const QString userIcon = QStringLiteral(":/res/head_2.jpg");
    const QVector<MsgInfo> msgList = ui->chatEdit->getMsgList();
    appendMsgList(msgList, role, userName, userIcon);
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
