/******************************************************************************
 *
 * @file       chatuserlist.cpp
 * @brief      Chat user list with hover scrollbar
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatuserlist.h"

#include <QDebug>
#include <QEvent>
#include <QScrollBar>
#include <QWheelEvent>

ChatUserList::ChatUserList(QWidget *parent)
    : QListWidget(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport()) {
        if (event->type() == QEvent::Enter) {
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else if (event->type() == QEvent::Wheel) {
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
            const int numDegrees = wheelEvent->angleDelta().y() / 8;
            const int numSteps = numDegrees / 15;

            verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps);

            QScrollBar *scrollBar = verticalScrollBar();
            if (scrollBar->maximum() - scrollBar->value() <= 0) {
                qDebug() << "load more chat user";
                emit sig_loading_chat_user();
            }

            return true;
        }
    }

    return QListWidget::eventFilter(watched, event);
}
