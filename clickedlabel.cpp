#include "clickedlabel.h"

#include <QEnterEvent>
#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget *parent)
    : QLabel(parent)
    , _curstate(ClickLbState::Normal)
{
}

void ClickedLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (_curstate == ClickLbState::Normal) {
            _curstate = ClickLbState::Selected;
            setProperty("state", _selected_press);
        } else {
            _curstate = ClickLbState::Normal;
            setProperty("state", _normal_press);
        }
        repolish(this);
        update();
        return;
    }
    QLabel::mousePressEvent(event);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (_curstate == ClickLbState::Normal) {
            setProperty("state", _normal_hover);
        } else {
            setProperty("state", _selected_hover);
        }
        repolish(this);
        update();
        emit clicked(text(), _curstate);
        return;
    }
    QLabel::mouseReleaseEvent(event);
}

void ClickedLabel::enterEvent(QEnterEvent *event)
{
    if (_curstate == ClickLbState::Normal) {
        setProperty("state", _normal_hover);
    } else {
        setProperty("state", _selected_hover);
    }
    repolish(this);
    update();
    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event)
{
    if (_curstate == ClickLbState::Normal) {
        setProperty("state", _normal);
    } else {
        setProperty("state", _selected);
    }
    repolish(this);
    update();
    QLabel::leaveEvent(event);
}

void ClickedLabel::SetState(const QString &normal,
                             const QString &hover,
                             const QString &press,
                             const QString &select,
                             const QString &select_hover,
                             const QString &select_press)
{
    Q_UNUSED(press);
    Q_UNUSED(select_press);
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;
    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;
    setProperty("state", normal);
    repolish(this);
}

ClickLbState ClickedLabel::GetCurState() const
{
    return _curstate;
}

void ClickedLabel::SetCurState(ClickLbState state)
{
    _curstate = state;
    if (_curstate == ClickLbState::Selected) {
        setProperty("state", _selected);
    } else {
        setProperty("state", _normal);
    }
    repolish(this);
    update();
}

void ClickedLabel::ResetNormalState()
{
    _curstate = ClickLbState::Normal;
    setProperty("state", _normal);
    repolish(this);
    update();
}
