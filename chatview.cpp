/******************************************************************************
 *
 * @file       chatview.cpp
 * @brief      Scrollable chat message list view
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatview.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

ChatView::ChatView(QWidget *parent)
    : QWidget(parent)
    , m_pVl(nullptr)
    , m_pScrollArea(nullptr)
    , isAppended(false)
{
    auto *pMainLayout = new QVBoxLayout();
    setLayout(pMainLayout);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);

    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName(QStringLiteral("chat_area"));
    pMainLayout->addWidget(m_pScrollArea);

    auto *w = new QWidget(this);
    w->setObjectName(QStringLiteral("chat_bg"));
    w->setAutoFillBackground(true);

    auto *pVLayout = new QVBoxLayout();
    pVLayout->addWidget(new QWidget(), 100000);
    w->setLayout(pVLayout);
    m_pScrollArea->setWidget(w);

    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    connect(pVScrollBar, &QScrollBar::rangeChanged, this, &ChatView::onVScrollBarMoved);

    auto *pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pVScrollBar, 0, Qt::AlignRight);
    pHLayout->setContentsMargins(0, 0, 0, 0);
    m_pScrollArea->setLayout(pHLayout);
    pVScrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    initStyleSheet();
}

void ChatView::appendChatItem(QWidget *item)
{
    auto *vl = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    if (!vl) {
        return;
    }
    vl->insertWidget(vl->count() - 1, item);
    isAppended = true;
}

void ChatView::prependChatItem(QWidget *item)
{
    Q_UNUSED(item);
}

void ChatView::insertChatItem(QWidget *before, QWidget *item)
{
    Q_UNUSED(before);
    Q_UNUSED(item);
}

void ChatView::removeAllItem()
{
    auto *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());
    if (!layout) {
        return;
    }

    const int count = layout->count();
    for (int i = 0; i < count - 1; ++i) {
        QLayoutItem *item = layout->takeAt(0);
        if (!item) {
            continue;
        }
        if (QWidget *widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
}

bool ChatView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_pScrollArea) {
        if (event->type() == QEvent::Enter) {
            m_pScrollArea->verticalScrollBar()->setHidden(
                m_pScrollArea->verticalScrollBar()->maximum() == 0);
        } else if (event->type() == QEvent::Leave) {
            m_pScrollArea->verticalScrollBar()->setHidden(true);
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ChatView::onVScrollBarMoved(int min, int max)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    if (isAppended) {
        QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        QTimer::singleShot(500, this, [this]() {
            isAppended = false;
        });
    }
}

void ChatView::initStyleSheet()
{
}
