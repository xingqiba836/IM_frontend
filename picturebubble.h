/******************************************************************************
 *
 * @file       picturebubble.h
 * @brief      Picture chat bubble widget
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "bubbleframe.h"

#include <QPixmap>

class PictureBubble : public BubbleFrame
{
    Q_OBJECT

public:
    PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent = nullptr);
};

#endif // PICTUREBUBBLE_H
