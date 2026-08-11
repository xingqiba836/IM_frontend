#include "statewidget.h"

#include <QEnterEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

StateWidget::StateWidget(QWidget *parent)
    : QWidget(parent)
    , _curstate(ClickLbState::Normal)
{
    setCursor(Qt::PointingHandCursor);
    AddRedPoint();
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void StateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (_curstate == ClickLbState::Selected) {
            QWidget::mousePressEvent(event);
            return;
        }

        if (_curstate == ClickLbState::Normal) {
            _curstate = ClickLbState::Selected;
            setProperty("state", _selected_press);
            repolish(this);
            update();
        }
        return;
    }
    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (_curstate == ClickLbState::Normal) {
            setProperty("state", _normal_hover);
        } else {
            setProperty("state", _selected_hover);
        }
        repolish(this);
        update();
        emit clicked();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void StateWidget::enterEvent(QEnterEvent *event)
{
    if (_curstate == ClickLbState::Normal) {
        setProperty("state", _normal_hover);
    } else {
        setProperty("state", _selected_hover);
    }
    repolish(this);
    update();
    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    if (_curstate == ClickLbState::Normal) {
        setProperty("state", _normal);
    } else {
        setProperty("state", _selected);
    }
    repolish(this);
    update();
    QWidget::leaveEvent(event);
}

void StateWidget::SetState(const QString &normal,
                           const QString &hover,
                           const QString &press,
                           const QString &select,
                           const QString &select_hover,
                           const QString &select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;
    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;
    setProperty("state", normal);
    repolish(this);
}

ClickLbState StateWidget::GetCurState() const
{
    return _curstate;
}

void StateWidget::ClearState()
{
    _curstate = ClickLbState::Normal;
    setProperty("state", _normal);
    repolish(this);
    update();
}

void StateWidget::SetSelected(bool bselected)
{
    if (bselected) {
        _curstate = ClickLbState::Selected;
        setProperty("state", _selected);
    } else {
        _curstate = ClickLbState::Normal;
        setProperty("state", _normal);
    }
    repolish(this);
    update();
}

void StateWidget::AddRedPoint()
{
    _red_point = new QLabel(this);
    _red_point->setObjectName(QStringLiteral("red_point"));
    auto *layout = new QVBoxLayout(this);
    _red_point->setAlignment(Qt::AlignCenter);
    layout->addWidget(_red_point);
    layout->setContentsMargins(0, 0, 0, 0);
    _red_point->setVisible(false);
}

void StateWidget::ShowRedPoint(bool show)
{
    if (_red_point) {
        _red_point->setVisible(show);
    }
}
