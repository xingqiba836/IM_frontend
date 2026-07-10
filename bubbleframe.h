/******************************************************************************
 *
 * @file       bubbleframe.h
 * @brief      Chat bubble frame base class
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H

#include "global.h"

#include <QFrame>

class QHBoxLayout;

class BubbleFrame : public QFrame
{
    Q_OBJECT

public:
    explicit BubbleFrame(ChatRole role, QWidget *parent = nullptr);

    void setMargin(int margin);
    void setWidget(QWidget *widget);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QHBoxLayout *m_pHLayout;
    ChatRole m_role;
    int m_margin;
};

#endif // BUBBLEFRAME_H
