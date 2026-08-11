/******************************************************************************
 *
 * @file       friendlabel.h
 * @brief      Friend tag label with close button
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include <QWidget>

namespace Ui {
class FriendLabel;
}

class FriendLabel : public QWidget
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel() override;

    void SetText(const QString &text);
    int Width() const;
    int Height() const;
    QString Text() const;

signals:
    void sig_close(const QString &name);

public slots:
    void slot_close();

private:
    Ui::FriendLabel *ui;
    QString _text;
    int _width = 0;
    int _height = 0;
};

#endif // FRIENDLABEL_H
