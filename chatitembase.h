/******************************************************************************
 *
 * @file       chatitembase.h
 * @brief      Chat message item with avatar and bubble
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include "global.h"

#include <QWidget>

class QLabel;

class ChatItemBase : public QWidget
{
    Q_OBJECT

public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);

    void setUserName(const QString &name);
    void setUserIcon(const QPixmap &icon);
    void setWidget(QWidget *widget);

private:
    ChatRole m_role;
    QLabel *m_pNameLabel;
    QLabel *m_pIconLabel;
    QWidget *m_pBubble;
};

#endif // CHATITEMBASE_H
