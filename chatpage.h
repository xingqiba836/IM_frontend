/******************************************************************************
 *
 * @file       chatpage.h
 * @brief      Chat message page
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATPAGE_H
#define CHATPAGE_H

#include "global.h"
#include "userdata.h"

#include <QWidget>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChatPage;
}
QT_END_NAMESPACE

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage() override;

    void SetUserInfo(const std::shared_ptr<UserInfo> &user_info);
    void AppendChatMsg(const std::shared_ptr<TextChatData> &msg);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void sig_append_send_chat_msg(const std::shared_ptr<TextChatData> &msg);

private slots:
    void on_send_btn_clicked();
    void on_receive_btn_clicked();

private:
    void appendMsgList(const QVector<MsgInfo> &msgList, ChatRole role,
                       const QString &userName, const QString &userIcon);

    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATPAGE_H
