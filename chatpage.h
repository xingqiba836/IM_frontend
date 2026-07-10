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

#include <QWidget>

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

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_send_btn_clicked();
    void on_receive_btn_clicked();

private:
    void appendMsgList(const QVector<MsgInfo> &msgList, ChatRole role,
                       const QString &userName, const QString &userIcon);

    Ui::ChatPage *ui;
};

#endif // CHATPAGE_H
