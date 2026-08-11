/******************************************************************************
 *
 * @file       findsuccessdlg.h
 * @brief      Dialog shown when a user search succeeds
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include "userdata.h"

#include <QDialog>
#include <QKeyEvent>
#include <memory>

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg() override;

    void SetSearchInfo(const std::shared_ptr<SearchInfo> &si);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void slot_add_friend_btn_clicked();
    void slot_close_btn_clicked();

private:
    Ui::FindSuccessDlg *ui;
    std::shared_ptr<SearchInfo> _si;
};

#endif // FINDSUCCESSDLG_H
