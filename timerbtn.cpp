#include "timerbtn.h"

#include <QMouseEvent>

TimerBtn::TimerBtn(QWidget *parent)
    : QPushButton(parent)
    , _counter(10)
{
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, [this]() {
        --_counter;
        if (_counter <= 0) {
            _timer->stop();
            _counter = 10;
            setText(QStringLiteral("获取"));
            setEnabled(true);
            return;
        }
        setText(QString::number(_counter));
    });
}

TimerBtn::~TimerBtn()
{
    _timer->stop();
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && isEnabled()) {
        setEnabled(false);
        setText(QString::number(_counter));
        _timer->start(1000);
        emit clicked();
    }
    QPushButton::mouseReleaseEvent(e);
}
