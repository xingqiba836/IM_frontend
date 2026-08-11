#include "applyfriendlist.h"

#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>

ApplyFriendList::ApplyFriendList(QWidget *parent)
    : QListWidget(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);
}

bool ApplyFriendList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport()) {
        if (event->type() == QEvent::Enter) {
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else if (event->type() == QEvent::MouseButtonPress) {
            emit sig_show_search(false);
        } else if (event->type() == QEvent::Wheel) {
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
            const int numDegrees = wheelEvent->angleDelta().y() / 8;
            const int numSteps = numDegrees / 15;
            verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps);
            return true;
        }
    }

    return QListWidget::eventFilter(watched, event);
}
