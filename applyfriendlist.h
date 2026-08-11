/******************************************************************************
 *
 * @file       applyfriendlist.h
 * @brief      Friend apply list with hover scrollbar
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef APPLYFRIENDLIST_H
#define APPLYFRIENDLIST_H

#include <QListWidget>

class ApplyFriendList : public QListWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendList(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void sig_show_search(bool show);
};

#endif // APPLYFRIENDLIST_H
