/******************************************************************************
 *
 * @file       chatuserlist.h
 * @brief      Chat user list with hover scrollbar
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>

class ChatUserList : public QListWidget
{
    Q_OBJECT

public:
    explicit ChatUserList(QWidget *parent = nullptr);

signals:
    void sig_loading_chat_user();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // CHATUSERLIST_H
