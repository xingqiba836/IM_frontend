/******************************************************************************
 *
 * @file       global.cpp
 * @brief      Global utilities implementation
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "global.h"

#include <QStyle>

std::function<void(QWidget *)> repolish = [](QWidget *w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QString gate_url_prefix = QString();
