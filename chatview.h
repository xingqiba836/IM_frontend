/******************************************************************************
 *
 * @file       chatview.h
 * @brief      Scrollable chat message list view
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QWidget>

class QScrollArea;
class QVBoxLayout;

class ChatView : public QWidget
{
    Q_OBJECT

public:
    explicit ChatView(QWidget *parent = nullptr);

    void appendChatItem(QWidget *item);
    void prependChatItem(QWidget *item);
    void insertChatItem(QWidget *before, QWidget *item);
    void removeAllItem();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onVScrollBarMoved(int min, int max);

private:
    void initStyleSheet();

    QVBoxLayout *m_pVl;
    QScrollArea *m_pScrollArea;
    bool isAppended;
};

#endif // CHATVIEW_H
