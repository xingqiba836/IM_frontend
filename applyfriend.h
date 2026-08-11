/******************************************************************************
 *
 * @file       applyfriend.h
 * @brief      Friend apply dialog
 *
 * @author     hexing
 * @date       2026/07/11
 *
 *****************************************************************************/
#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include "clickedlabel.h"
#include "friendlabel.h"
#include "global.h"
#include "userdata.h"

#include <QDialog>
#include <QMap>
#include <QPoint>
#include <QShowEvent>
#include <QVector>

#include <memory>

namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend() override;

    void InitTipLbs();
    void AddTipLbs(ClickedLabel *lb,
                   QPoint cur_point,
                   QPoint &next_point,
                   int text_width,
                   int text_height);
    void SetSearchInfo(const std::shared_ptr<SearchInfo> &si);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

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
    void updateGridWidgetSize();
    int effectiveGridWidth() const;

    Ui::ApplyFriend *ui;
    QMap<QString, ClickedLabel *> _add_labels;
    QVector<QString> _add_label_keys;
    QPoint _label_point;
    QMap<QString, FriendLabel *> _friend_labels;
    QVector<QString> _friend_label_keys;
    QVector<QString> _tip_data;
    QPoint _tip_cur_point;
    std::shared_ptr<SearchInfo> _si;
    bool _closing = false;
};

#endif // APPLYFRIEND_H
