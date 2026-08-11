/******************************************************************************
 *
 * @file       userinfopage.h
 * @brief      Current user profile page in chat dialog
 *
 * @author     hexing
 * @date       2026/08/12
 *
 *****************************************************************************/
#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>

namespace Ui {
class UserInfoPage;
}

class UserInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);
    ~UserInfoPage() override;

private slots:
    void slot_up_btn_clicked();
    void slot_submit_btn_clicked();

private:
    Ui::UserInfoPage *ui;
};

#endif // USERINFOPAGE_H
