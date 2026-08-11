/******************************************************************************
 *
 * @file       findfaildlg.h
 * @brief      Dialog shown when a user search fails
 *
 * @author     hexing
 * @date       2026/08/12
 *
 *****************************************************************************/
#ifndef FINDFAILDLG_H
#define FINDFAILDLG_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class FindFailDlg;
}

class FindFailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindFailDlg(QWidget *parent = nullptr);
    ~FindFailDlg() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void slot_fail_sure_btn_clicked();

private:
    Ui::FindFailDlg *ui;
};

#endif // FINDFAILDLG_H
