/******************************************************************************
 *
 * @file       customizeedit.cpp
 * @brief      Search line edit with length limit
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "customizeedit.h"

#include <QFocusEvent>

CustomizeEdit::CustomizeEdit(QWidget *parent)
    : QLineEdit(parent)
    , _max_len(0)
{
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitTextLength);
}

void CustomizeEdit::SetMaxLength(int maxLen)
{
    _max_len = maxLen;
}

void CustomizeEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit sig_foucus_out();
}

void CustomizeEdit::limitTextLength(const QString &text)
{
    if (_max_len <= 0) {
        return;
    }

    const QByteArray byteArray = text.toUtf8();
    if (byteArray.size() > _max_len) {
        setText(QString::fromUtf8(byteArray.left(_max_len)));
    }
}
