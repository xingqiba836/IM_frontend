#include "authenfriend.h"
#include "clickedbtn.h"
#include "clickedoncelabel.h"
#include "customizeedit.h"
#include "global.h"
#include "tcpmgr.h"
#include "ui_authenfriend.h"
#include "usermgr.h"

#include <QDebug>
#include <QFontMetrics>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>

#include <algorithm>

AuthenFriend::AuthenFriend(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuthenFriend)
    , _label_point(2, 6)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Window);
    setObjectName(QStringLiteral("AuthenFriend"));
    setModal(true);
    ui->lb_ed->setPlaceholderText(QStringLiteral("搜索、添加标签"));
    ui->back_ed->setPlaceholderText(QStringLiteral("燃烧的胸毛"));

    ui->lb_ed->SetMaxLength(21);
    ui->lb_ed->move(2, 2);
    ui->lb_ed->setFixedHeight(20);
    ui->lb_ed->setMaxLength(10);
    ui->input_tip_wid->hide();

    _tip_cur_point = QPoint(5, 5);

    _tip_data = { QStringLiteral("同学"), QStringLiteral("家人"), QStringLiteral("菜鸟教程"),
                  QStringLiteral("C++ Primer"), QStringLiteral("Rust 程序设计"),
                  QStringLiteral("父与子学Python"), QStringLiteral("nodejs开发指南"),
                  QStringLiteral("go 语言开发指南"), QStringLiteral("游戏伙伴"),
                  QStringLiteral("金融投资"), QStringLiteral("微信读书"), QStringLiteral("拼多多拼友") };

    connect(ui->more_lb, &ClickedOnceLabel::clicked, this, &AuthenFriend::ShowMoreLabel);
    InitTipLbs();
    connect(ui->lb_ed, &CustomizeEdit::returnPressed, this, &AuthenFriend::SlotLabelEnter);
    connect(ui->lb_ed, &CustomizeEdit::textChanged, this, &AuthenFriend::SlotLabelTextChange);
    connect(ui->lb_ed, &CustomizeEdit::editingFinished, this, &AuthenFriend::SlotLabelEditFinished);
    connect(ui->tip_lb, &ClickedOnceLabel::clicked, this, &AuthenFriend::SlotAddFirendLabelByClickTip);

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->sure_btn->SetState(QStringLiteral("normal"),
                           QStringLiteral("hover"),
                           QStringLiteral("press"));
    ui->cancel_btn->SetState(QStringLiteral("normal"),
                             QStringLiteral("hover"),
                             QStringLiteral("press"));
    ui->cancel_btn->setFocusPolicy(Qt::NoFocus);
    ui->sure_btn->setFocusPolicy(Qt::NoFocus);
    connect(ui->cancel_btn, &ClickedBtn::clicked, this, &AuthenFriend::SlotApplyCancel);
    connect(ui->sure_btn, &ClickedBtn::clicked, this, &AuthenFriend::SlotApplySure);
    ui->lb_ed->setFocusPolicy(Qt::ClickFocus);
}

AuthenFriend::~AuthenFriend()
{
    qDebug() << "AuthenFriend destruct";
    delete ui;
}

void AuthenFriend::InitTipLbs()
{
    int lines = 1;
    for (int i = 0; i < _tip_data.size(); ++i) {
        auto *lb = new ClickedLabel(ui->lb_list);
        lb->SetState(QStringLiteral("normal"), QStringLiteral("hover"), QStringLiteral("pressed"),
                     QStringLiteral("selected_normal"), QStringLiteral("selected_hover"),
                     QStringLiteral("selected_pressed"));
        lb->setObjectName(QStringLiteral("tipslb"));
        lb->setText(_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &AuthenFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font());
        const int textWidth = fontMetrics.horizontalAdvance(lb->text());
        const int textHeight = fontMetrics.height();

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            ++lines;
            if (lines > 2) {
                delete lb;
                return;
            }

            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }

        auto next_point = _tip_cur_point;
        AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
        _tip_cur_point = next_point;
    }
}

void AuthenFriend::AddTipLbs(ClickedLabel *lb,
                             QPoint cur_point,
                             QPoint &next_point,
                             int text_width,
                             int text_height)
{
    Q_UNUSED(text_height);
    lb->move(cur_point);
    lb->show();
    _add_labels.insert(lb->text(), lb);
    _add_label_keys.push_back(lb->text());
    next_point.setX(lb->pos().x() + text_width + 15);
    next_point.setY(lb->pos().y());
}

bool AuthenFriend::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->scrollArea && event->type() == QEvent::Enter) {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    } else if (obj == ui->scrollArea && event->type() == QEvent::Leave) {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void AuthenFriend::SetApplyInfo(const std::shared_ptr<ApplyInfo> &apply_info)
{
    _apply_info = apply_info;
    if (!apply_info) {
        return;
    }

    ui->back_ed->setPlaceholderText(apply_info->_name);
}

int AuthenFriend::effectiveGridWidth() const
{
    int width = ui->gridWidget->width();
    if (width < MIN_APPLY_LABEL_ED_LEN) {
        width = ui->apply_wid->width() - 8;
    }
    if (width < MIN_APPLY_LABEL_ED_LEN) {
        width = 310;
    }
    return width;
}

void AuthenFriend::ShowMoreLabel()
{
    qDebug() << "receive more label clicked";
    ui->more_lb_wid->hide();

    ui->lb_list->setFixedWidth(325);
    _tip_cur_point = QPoint(5, 5);
    auto next_point = _tip_cur_point;
    int textWidth = 0;
    int textHeight = 0;

    for (const auto &added_key : _add_label_keys) {
        auto *added_lb = _add_labels[added_key];

        QFontMetrics fontMetrics(added_lb->font());
        textWidth = fontMetrics.horizontalAdvance(added_lb->text());
        textHeight = fontMetrics.height();

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }
        added_lb->move(_tip_cur_point);

        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(_tip_cur_point.y());
        _tip_cur_point = next_point;
    }

    for (int i = 0; i < _tip_data.size(); ++i) {
        if (_add_labels.contains(_tip_data[i])) {
            continue;
        }

        auto *lb = new ClickedLabel(ui->lb_list);
        lb->SetState(QStringLiteral("normal"), QStringLiteral("hover"), QStringLiteral("pressed"),
                     QStringLiteral("selected_normal"), QStringLiteral("selected_hover"),
                     QStringLiteral("selected_pressed"));
        lb->setObjectName(QStringLiteral("tipslb"));
        lb->setText(_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &AuthenFriend::SlotChangeFriendLabelByTip);

        QFontMetrics fontMetrics(lb->font());
        textWidth = fontMetrics.horizontalAdvance(lb->text());
        textHeight = fontMetrics.height();

        if (_tip_cur_point.x() + textWidth + tip_offset > ui->lb_list->width()) {
            _tip_cur_point.setX(tip_offset);
            _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
        }

        next_point = _tip_cur_point;
        AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
        _tip_cur_point = next_point;
    }

    const int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void AuthenFriend::resetLabels()
{
    const int max_width = effectiveGridWidth();
    int label_height = 0;
    for (auto iter = _friend_labels.begin(); iter != _friend_labels.end(); ++iter) {
        if (_label_point.x() + iter.value()->width() > max_width) {
            _label_point.setY(_label_point.y() + iter.value()->height() + 6);
            _label_point.setX(2);
        }

        iter.value()->move(_label_point);
        iter.value()->show();
        iter.value()->raise();

        _label_point.setX(_label_point.x() + iter.value()->width() + 2);
        label_height = iter.value()->height();
    }

    if (_friend_labels.isEmpty()) {
        ui->lb_ed->move(_label_point);
        return;
    }

    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > effectiveGridWidth()) {
        ui->lb_ed->move(2, _label_point.y() + label_height + 6);
    } else {
        ui->lb_ed->move(_label_point);
    }
}

void AuthenFriend::addLabel(const QString &name)
{
    if (_friend_labels.contains(name)) {
        ui->lb_ed->clear();
        return;
    }

    auto *tmplabel = new FriendLabel(ui->gridWidget);
    tmplabel->SetText(name);
    tmplabel->setObjectName(QStringLiteral("FriendLabel"));

    const int max_width = effectiveGridWidth();
    if (_label_point.x() + tmplabel->width() > max_width) {
        _label_point.setY(_label_point.y() + tmplabel->height() + 6);
        _label_point.setX(2);
    }

    tmplabel->move(_label_point);
    tmplabel->show();
    tmplabel->raise();
    _friend_labels[tmplabel->Text()] = tmplabel;
    _friend_label_keys.push_back(tmplabel->Text());

    connect(tmplabel, &FriendLabel::sig_close, this, &AuthenFriend::SlotRemoveFriendLabel);

    _label_point.setX(_label_point.x() + tmplabel->width() + 2);

    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > effectiveGridWidth()) {
        ui->lb_ed->move(2, _label_point.y() + tmplabel->height() + 2);
    } else {
        ui->lb_ed->move(_label_point);
    }

    ui->lb_ed->clear();

    if (ui->gridWidget->minimumHeight() < _label_point.y() + tmplabel->height() + 6) {
        ui->gridWidget->setMinimumHeight(_label_point.y() + tmplabel->height() + 6);
    }
}

void AuthenFriend::SlotLabelEnter()
{
    if (ui->lb_ed->text().isEmpty()) {
        return;
    }

    const QString text = ui->lb_ed->text();
    addLabel(text);
    ui->input_tip_wid->hide();

    if (!std::any_of(_tip_data.begin(), _tip_data.end(),
                     [&text](const QString &item) { return item == text; })) {
        _tip_data.push_back(text);
    }

    const auto find_add = _add_labels.find(text);
    if (find_add != _add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
    }
}

void AuthenFriend::SlotRemoveFriendLabel(const QString &name)
{
    qDebug() << "receive close signal";

    _label_point = QPoint(2, 6);

    const auto find_iter = _friend_labels.find(name);
    if (find_iter == _friend_labels.end()) {
        return;
    }

    auto find_key = _friend_label_keys.end();
    for (auto iter = _friend_label_keys.begin(); iter != _friend_label_keys.end(); ++iter) {
        if (*iter == name) {
            find_key = iter;
            break;
        }
    }

    if (find_key != _friend_label_keys.end()) {
        _friend_label_keys.erase(find_key);
    }

    delete find_iter.value();
    _friend_labels.erase(find_iter);
    resetLabels();

    const auto find_add = _add_labels.find(name);
    if (find_add != _add_labels.end()) {
        find_add.value()->ResetNormalState();
    }
}

void AuthenFriend::SlotChangeFriendLabelByTip(const QString &lbtext, ClickLbState state)
{
    if (!_add_labels.contains(lbtext)) {
        return;
    }

    if (state == ClickLbState::Selected) {
        addLabel(lbtext);
        return;
    }

    if (state == ClickLbState::Normal) {
        SlotRemoveFriendLabel(lbtext);
    }
}

void AuthenFriend::SlotLabelTextChange(const QString &text)
{
    if (text.isEmpty()) {
        ui->tip_lb->setText(QString());
        ui->input_tip_wid->hide();
        return;
    }

    const auto iter = std::find(_tip_data.begin(), _tip_data.end(), text);
    if (iter == _tip_data.end()) {
        ui->tip_lb->setText(add_prefix + text);
        ui->input_tip_wid->show();
        return;
    }

    ui->tip_lb->setText(text);
    ui->input_tip_wid->show();
}

void AuthenFriend::SlotLabelEditFinished()
{
    ui->input_tip_wid->hide();
}

void AuthenFriend::SlotAddFirendLabelByClickTip(const QString &text)
{
    QString labelText = text;
    const int index = labelText.indexOf(add_prefix);
    if (index != -1) {
        labelText = labelText.mid(index + add_prefix.length());
    }
    addLabel(labelText);

    if (!std::any_of(_tip_data.begin(), _tip_data.end(),
                     [&labelText](const QString &item) { return item == labelText; })) {
        _tip_data.push_back(labelText);
    }

    const auto find_add = _add_labels.find(labelText);
    if (find_add != _add_labels.end()) {
        find_add.value()->SetCurState(ClickLbState::Selected);
        return;
    }

    auto *lb = new ClickedLabel(ui->lb_list);
    lb->SetState(QStringLiteral("normal"), QStringLiteral("hover"), QStringLiteral("pressed"),
                 QStringLiteral("selected_normal"), QStringLiteral("selected_hover"),
                 QStringLiteral("selected_pressed"));
    lb->setObjectName(QStringLiteral("tipslb"));
    lb->setText(labelText);
    connect(lb, &ClickedLabel::clicked, this, &AuthenFriend::SlotChangeFriendLabelByTip);

    QFontMetrics fontMetrics(lb->font());
    const int textWidth = fontMetrics.horizontalAdvance(lb->text());
    const int textHeight = fontMetrics.height();

    if (_tip_cur_point.x() + textWidth + tip_offset + 3 > ui->lb_list->width()) {
        _tip_cur_point.setX(5);
        _tip_cur_point.setY(_tip_cur_point.y() + textHeight + 15);
    }

    auto next_point = _tip_cur_point;
    AddTipLbs(lb, _tip_cur_point, next_point, textWidth, textHeight);
    _tip_cur_point = next_point;

    const int diff_height = next_point.y() + textHeight + tip_offset - ui->lb_list->height();
    ui->lb_list->setFixedHeight(next_point.y() + textHeight + tip_offset);
    lb->SetCurState(ClickLbState::Selected);
    ui->scrollcontent->setFixedHeight(ui->scrollcontent->height() + diff_height);
}

void AuthenFriend::SlotApplySure()
{
    if (_closing || !_apply_info) {
        return;
    }
    _closing = true;

    qDebug() << "Slot Apply Sure";
    ui->lb_ed->clearFocus();

    QJsonObject jsonObj;
    jsonObj[QStringLiteral("fromuid")] = UserMgr::GetInstance()->GetUid();
    jsonObj[QStringLiteral("touid")] = _apply_info->_uid;

    QString back_name;
    if (ui->back_ed->text().isEmpty()) {
        back_name = ui->back_ed->placeholderText();
    } else {
        back_name = ui->back_ed->text();
    }
    jsonObj[QStringLiteral("back")] = back_name;

    const QJsonDocument doc(jsonObj);
    emit TcpMgr::GetInstance()->sig_send_data(
        ReqId::ID_AUTH_FRIEND_REQ,
        doc.toJson(QJsonDocument::Compact));

    hide();
    deleteLater();
}

void AuthenFriend::SlotApplyCancel()
{
    if (_closing) {
        return;
    }
    _closing = true;

    ui->lb_ed->clearFocus();
    hide();
    deleteLater();
}
