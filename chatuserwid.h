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

#include <QSize>
#include <QString>

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

private:
    Ui::ChatUserWid *ui;
    QString _name;
    QString _head;
    QString _msg;
};

#endif // CHATUSERWID_H
