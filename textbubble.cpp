/******************************************************************************
 *
 * @file       textbubble.cpp
 * @brief      Text chat bubble widget
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "textbubble.h"

#include <QEvent>
#include <QFont>
#include <QFontMetricsF>
#include <QLayout>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextLayout>

TextBubble::TextBubble(ChatRole role, const QString &text, QWidget *parent)
    : BubbleFrame(role, parent)
    , m_pTextEdit(new QTextEdit())
{
    m_pTextEdit->setReadOnly(true);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->installEventFilter(this);

    QFont font(QStringLiteral("Microsoft YaHei"));
    font.setPointSize(12);
    m_pTextEdit->setFont(font);

    setPlainText(text);
    setWidget(m_pTextEdit);
    initStyleSheet();
}

bool TextBubble::eventFilter(QObject *watched, QEvent *event)
{
    if (m_pTextEdit == watched && event->type() == QEvent::Paint) {
        adjustTextHeight();
    }
    return BubbleFrame::eventFilter(watched, event);
}

void TextBubble::setPlainText(const QString &text)
{
    m_pTextEdit->setPlainText(text);

    const qreal docMargin = m_pTextEdit->document()->documentMargin();
    const int marginLeft = layout()->contentsMargins().left();
    const int marginRight = layout()->contentsMargins().right();
    QFontMetricsF fm(m_pTextEdit->font());
    QTextDocument *doc = m_pTextEdit->document();
    int maxWidth = 0;

    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
        const int txtW = int(fm.horizontalAdvance(it.text()));
        maxWidth = maxWidth < txtW ? txtW : maxWidth;
    }

    setMaximumWidth(maxWidth + docMargin * 2 + (marginLeft + marginRight));
}

void TextBubble::adjustTextHeight()
{
    const qreal docMargin = m_pTextEdit->document()->documentMargin();
    QTextDocument *doc = m_pTextEdit->document();
    qreal textHeight = 0;

    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
        QTextLayout *pLayout = it.layout();
        const QRectF textRect = pLayout->boundingRect();
        textHeight += textRect.height();
    }

    const int vMargin = layout()->contentsMargins().top();
    setFixedHeight(textHeight + docMargin * 2 + vMargin * 2);
}

void TextBubble::initStyleSheet()
{
    m_pTextEdit->setStyleSheet(QStringLiteral("QTextEdit{background:transparent;border:none}"));
}
