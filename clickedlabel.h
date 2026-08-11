#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include "global.h"

#include <QLabel>

class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickedLabel(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void SetState(const QString &normal = QString(),
                  const QString &hover = QString(),
                  const QString &press = QString(),
                  const QString &select = QString(),
                  const QString &select_hover = QString(),
                  const QString &select_press = QString());

    ClickLbState GetCurState() const;
    void SetCurState(ClickLbState state);
    void ResetNormalState();

signals:
    void clicked(const QString &text, ClickLbState state);

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
};

#endif // CLICKEDLABEL_H
