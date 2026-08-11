/******************************************************************************
 *
 * @file       authenfriend.h
 * @brief      Friend authentication dialog
 *
 * @author     hexing
 * @date       2026/08/12
 *
 *****************************************************************************/
#ifndef AUTHENFRIEND_H
#define AUTHENFRIEND_H

#include "clickedlabel.h"
#include "friendlabel.h"
#include "global.h"
#include "userdata.h"

#include <QDialog>
#include <QMap>
#include <QPoint>
#include <QVector>

#include <memory>

namespace Ui {
class AuthenFriend;
}

class AuthenFriend : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenFriend(QWidget *parent = nullptr);
    ~AuthenFriend() override;

    void InitTipLbs();
    void AddTipLbs(ClickedLabel *lb,
                   QPoint cur_point,
                   QPoint &next_point,
                   int text_width,
                   int text_height);
    void SetApplyInfo(const std::shared_ptr<ApplyInfo> &apply_info);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void ShowMoreLabel();
    void SlotLabelEnter();
    void SlotRemoveFriendLabel(const QString &name);
    void SlotChangeFriendLabelByTip(const QString &lbtext, ClickLbState state);
    void SlotLabelTextChange(const QString &text);
    void SlotLabelEditFinished();
    void SlotAddFirendLabelByClickTip(const QString &text);
    void SlotApplySure();
    void SlotApplyCancel();

private:
    void resetLabels();
    void addLabel(const QString &name);
    int effectiveGridWidth() const;

    Ui::AuthenFriend *ui;
    QMap<QString, ClickedLabel *> _add_labels;
    QVector<QString> _add_label_keys;
    QPoint _label_point;
    QMap<QString, FriendLabel *> _friend_labels;
    QVector<QString> _friend_label_keys;
    QVector<QString> _tip_data;
    QPoint _tip_cur_point;
    std::shared_ptr<ApplyInfo> _apply_info;
    bool _closing = false;
};

#endif // AUTHENFRIEND_H
