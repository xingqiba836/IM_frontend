#include "friendlabel.h"
#include "ui_friendlabel.h"

#include <QFontMetrics>

FriendLabel::FriendLabel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendLabel)
{
    ui->setupUi(this);
    ui->close_lb->SetState(QStringLiteral("normal"),
                           QStringLiteral("hover"),
                           QStringLiteral("pressed"),
                           QStringLiteral("selected_normal"),
                           QStringLiteral("selected_hover"),
                           QStringLiteral("selected_pressed"));
    connect(ui->close_lb, &ClickedLabel::clicked, this, [this](const QString &, ClickLbState) {
        slot_close();
    });
}

FriendLabel::~FriendLabel()
{
    delete ui;
}

void FriendLabel::SetText(const QString &text)
{
    _text = text;
    ui->tip_lb->setText(_text);
    ui->tip_lb->setWordWrap(false);

    QFontMetrics fontMetrics(ui->tip_lb->font());
    const int textWidth = fontMetrics.horizontalAdvance(_text);
    const int textHeight = fontMetrics.height();

    ui->tip_lb->setMinimumSize(textWidth, textHeight);
    ui->tip_lb->setMaximumSize(textWidth, textHeight);

    adjustSize();
    _width = width();
    _height = height();
}

int FriendLabel::Width() const
{
    return _width;
}

int FriendLabel::Height() const
{
    return _height;
}

QString FriendLabel::Text() const
{
    return _text;
}

void FriendLabel::slot_close()
{
    emit sig_close(_text);
}
