/******************************************************************************
 *
 * @file       chatitembase.cpp
 * @brief      Chat message item with avatar and bubble
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatitembase.h"

#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>

ChatItemBase::ChatItemBase(ChatRole role, QWidget *parent)
    : QWidget(parent)
    , m_role(role)
    , m_pNameLabel(new QLabel())
    , m_pIconLabel(new QLabel())
    , m_pBubble(new QWidget())
{
    m_pNameLabel->setObjectName(QStringLiteral("chat_user_name"));
    QFont font(QStringLiteral("Microsoft YaHei"));
    font.setPointSize(9);
    m_pNameLabel->setFont(font);
    m_pNameLabel->setFixedHeight(20);

    m_pIconLabel->setScaledContents(true);
    m_pIconLabel->setFixedSize(42, 42);

    auto *pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);
    pGLayout->setHorizontalSpacing(3);
    pGLayout->setContentsMargins(3, 3, 3, 3);

    auto *pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    if (m_role == ChatRole::Self) {
        m_pNameLabel->setContentsMargins(0, 0, 8, 0);
        m_pNameLabel->setAlignment(Qt::AlignRight);
        pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);
        pGLayout->addWidget(m_pIconLabel, 0, 2, 2, 1, Qt::AlignTop);
        pGLayout->addItem(pSpacer, 1, 0, 1, 1);
        pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);
        pGLayout->setColumnStretch(0, 2);
        pGLayout->setColumnStretch(1, 3);
    } else {
        m_pNameLabel->setContentsMargins(8, 0, 0, 0);
        m_pNameLabel->setAlignment(Qt::AlignLeft);
        pGLayout->addWidget(m_pIconLabel, 0, 0, 2, 1, Qt::AlignTop);
        pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);
        pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);
        pGLayout->addItem(pSpacer, 2, 2, 1, 1);
        pGLayout->setColumnStretch(1, 3);
        pGLayout->setColumnStretch(2, 2);
    }

    setLayout(pGLayout);
}

void ChatItemBase::setUserName(const QString &name)
{
    m_pNameLabel->setText(name);
}

void ChatItemBase::setUserIcon(const QPixmap &icon)
{
    m_pIconLabel->setPixmap(icon);
}

void ChatItemBase::setWidget(QWidget *widget)
{
    auto *pGLayout = qobject_cast<QGridLayout *>(layout());
    if (!pGLayout) {
        return;
    }
    pGLayout->replaceWidget(m_pBubble, widget);
    delete m_pBubble;
    m_pBubble = widget;
}
