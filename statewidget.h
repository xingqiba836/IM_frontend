/******************************************************************************
 *
 * @file       statewidget.h
 * @brief      Sidebar button with selectable states and red dot
 *
 * @author     hexing
 * @date       2026/07/08
 *
 *****************************************************************************/
#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include "global.h"

#include <QLabel>
#include <QWidget>

class StateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(const QString &normal = QString(),
                  const QString &hover = QString(),
                  const QString &press = QString(),
                  const QString &select = QString(),
                  const QString &select_hover = QString(),
                  const QString &select_press = QString());

    ClickLbState GetCurState() const;
    void ClearState();
    void SetSelected(bool bselected);
    void AddRedPoint();
    void ShowRedPoint(bool show = true);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
    QLabel *_red_point = nullptr;
};

#endif // STATEWIDGET_H
