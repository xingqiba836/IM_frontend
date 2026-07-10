/******************************************************************************
 *
 * @file       picturebubble.cpp
 * @brief      Picture chat bubble widget
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "picturebubble.h"

#include <QLabel>
#include <QLayout>

namespace {
const int PIC_MAX_WIDTH = 160;
const int PIC_MAX_HEIGHT = 90;
}

PictureBubble::PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent)
    : BubbleFrame(role, parent)
{
    auto *lb = new QLabel();
    lb->setScaledContents(true);
    const QPixmap pix = picture.scaled(QSize(PIC_MAX_WIDTH, PIC_MAX_HEIGHT), Qt::KeepAspectRatio);
    lb->setPixmap(pix);
    setWidget(lb);

    const int leftMargin = layout()->contentsMargins().left();
    const int rightMargin = layout()->contentsMargins().right();
    const int vMargin = layout()->contentsMargins().bottom();
    setFixedSize(pix.width() + leftMargin + rightMargin, pix.height() + vMargin * 2);
}
