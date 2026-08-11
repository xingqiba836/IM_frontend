/******************************************************************************
 *
 * @file       applyfriendpage.h
 * @brief      Friend apply list page
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include "userdata.h"

#include <QWidget>

#include <memory>
#include <unordered_map>

namespace Ui {
class ApplyFriendPage;
}

class ApplyFriendItem;

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage() override;

    void AddNewApply(const std::shared_ptr<AddFriendApply> &apply);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void loadApplyList();

    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem *> _unauth_items;

public slots:
    void slot_auth_rsp(const std::shared_ptr<AuthRsp> &auth_rsp);

signals:
    void sig_show_search(bool show);
};

#endif // APPLYFRIENDPAGE_H
