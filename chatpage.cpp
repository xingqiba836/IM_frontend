/******************************************************************************
 *
 * @file       chatpage.cpp
 * @brief      Chat message page
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatpage.h"
#include "ui_chatpage.h"

#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    setAutoFillBackground(true);

    ui->receive_btn->SetState(QStringLiteral("normal"),
                              QStringLiteral("hover"),
                              QStringLiteral("press"));
    ui->send_btn->SetState(QStringLiteral("normal"),
                           QStringLiteral("hover"),
                           QStringLiteral("press"));

    ui->emo_lb->SetState(QStringLiteral("normal"),
                         QStringLiteral("hover"),
                         QStringLiteral("press"),
                         QStringLiteral("normal"),
                         QStringLiteral("hover"),
                         QStringLiteral("press"));
    ui->file_lb->SetState(QStringLiteral("normal"),
                          QStringLiteral("hover"),
                          QStringLiteral("press"),
                          QStringLiteral("normal"),
                          QStringLiteral("hover"),
                          QStringLiteral("press"));

    ui->title_lb->setText(tr("hexing"));
    ui->chatEdit->setPlainText(tr("欢迎使用 IM_frontend 聊天界面。"));

    const QPixmap headSrc(QStringLiteral(":/res/wechat.png"));
    if (!headSrc.isNull()) {
        ui->head_lb->setPixmap(headSrc.scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->head_lb->setScaledContents(true);
    }
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
