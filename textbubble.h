/******************************************************************************
 *
 * @file       textbubble.h
 * @brief      Text chat bubble widget
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include "bubbleframe.h"

class QTextEdit;

class TextBubble : public BubbleFrame
{
    Q_OBJECT

public:
    TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void adjustTextHeight();
    void setPlainText(const QString &text);
    void initStyleSheet();

    QTextEdit *m_pTextEdit;
};

#endif // TEXTBUBBLE_H
