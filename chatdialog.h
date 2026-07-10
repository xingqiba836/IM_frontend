/******************************************************************************
 *
 * @file       chatdialog.h
 * @brief      Main chat interface
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include "global.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChatDialog;
}
QT_END_NAMESPACE

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog() override;

private slots:
    void slot_loading_chat_user();

private:
    void setupSearchEdit();
    void ShowSearch(bool bsearch);
    void addChatUserList();

    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
};

#endif // CHATDIALOG_H
