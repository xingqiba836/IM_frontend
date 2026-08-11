/******************************************************************************
 *
 * @file       chatuserwid.h
 * @brief      Chat list item widget
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include "listitembase.h"
#include "userdata.h"

#include <QSize>
#include <QString>

#include <memory>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChatUserWid;
}
QT_END_NAMESPACE

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid() override;

    QSize sizeHint() const override;

    void SetInfo(const QString &name, const QString &head, const QString &msg);
    void SetInfo(const std::shared_ptr<UserInfo> &user_info);
    std::shared_ptr<UserInfo> GetUserInfo() const;
    void ShowRedPoint(bool bshow = false);
    void updateLastMsg(const std::vector<std::shared_ptr<TextChatData>> &msgs);

private:
    Ui::ChatUserWid *ui;
    QString _name;
    QString _head;
    QString _msg;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATUSERWID_H
