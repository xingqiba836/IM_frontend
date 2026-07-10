/******************************************************************************
 *
 * @file       chatpage.h
 * @brief      Chat message page
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChatPage;
}
QT_END_NAMESPACE

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::ChatPage *ui;
};

#endif // CHATPAGE_H
