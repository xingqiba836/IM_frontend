/******************************************************************************
 *
 * @file       bubbleframe.cpp
 * @brief      Chat bubble frame base class
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "bubbleframe.h"

#include <QHBoxLayout>
#include <QPainter>

namespace {
const int WIDTH_SANJIAO = 8;
}

BubbleFrame::BubbleFrame(ChatRole role, QWidget *parent)
    : QFrame(parent)
    , m_pHLayout(new QHBoxLayout())
    , m_role(role)
    , m_margin(3)
{
    if (m_role == ChatRole::Self) {
        m_pHLayout->setContentsMargins(m_margin, m_margin, WIDTH_SANJIAO + m_margin, m_margin);
    } else {
        m_pHLayout->setContentsMargins(WIDTH_SANJIAO + m_margin, m_margin, m_margin, m_margin);
    }
    setLayout(m_pHLayout);
}

void BubbleFrame::setMargin(int margin)
{
    Q_UNUSED(margin);
}

void BubbleFrame::setWidget(QWidget *widget)
{
    if (m_pHLayout->count() > 0) {
        return;
    }
    m_pHLayout->addWidget(widget);
}

void BubbleFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    if (m_role == ChatRole::Other) {
        const QColor bkColor(Qt::white);
        painter.setBrush(QBrush(bkColor));
        const QRect bkRect(WIDTH_SANJIAO, 0, width() - WIDTH_SANJIAO, height());
        painter.drawRoundedRect(bkRect, 5, 5);

        const QPointF points[3] = {
            QPointF(bkRect.x(), 12),
            QPointF(bkRect.x(), 10 + WIDTH_SANJIAO + 2),
            QPointF(bkRect.x() - WIDTH_SANJIAO, 10 + WIDTH_SANJIAO - WIDTH_SANJIAO / 2.0),
        };
        painter.drawPolygon(points, 3);
    } else {
        const QColor bkColor(158, 234, 106);
        painter.setBrush(QBrush(bkColor));
        const QRect bkRect(0, 0, width() - WIDTH_SANJIAO, height());
        painter.drawRoundedRect(bkRect, 5, 5);

        const QPointF points[3] = {
            QPointF(bkRect.x() + bkRect.width(), 12),
            QPointF(bkRect.x() + bkRect.width(), 12 + WIDTH_SANJIAO + 2),
            QPointF(bkRect.x() + bkRect.width() + WIDTH_SANJIAO,
                    10 + WIDTH_SANJIAO - WIDTH_SANJIAO / 2.0),
        };
        painter.drawPolygon(points, 3);
    }

    QFrame::paintEvent(event);
}
