/******************************************************************************
 *
 * @file       searchlist.h
 * @brief      Search result list with hover scrollbar
 *
 * @author     hexing
 * @date       2026/07/08
 *
 *****************************************************************************/
#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include "userdata.h"

#include <QDialog>
#include <QEvent>
#include <QListWidget>
#include <QPoint>
#include <QScrollBar>
#include <QWheelEvent>

#include <memory>

class CustomizeEdit;
class LoadingDlg;

class SearchList : public QListWidget
{
    Q_OBJECT

public:
    explicit SearchList(QWidget *parent = nullptr);

    void CloseFindDlg();
    void SetSearchEdit(QWidget *edit);
    bool IsClickOnFindDlg(const QPoint &globalPos) const;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void waitPending(bool pending = true);
    void addTipItem();

    bool _send_pending = false;
    QWidget *_search_edit = nullptr;
    LoadingDlg *_loadingDialog = nullptr;
    std::shared_ptr<QDialog> _find_dlg;

private slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_user_search(std::shared_ptr<SearchInfo> si);

signals:
    void sig_jump_chat_item(std::shared_ptr<SearchInfo> si);
};

#endif // SEARCHLIST_H
