#include "searchlist.h"

#include "adduseritem.h"
#include "customizeedit.h"
#include "findfaildlg.h"
#include "findsuccessdlg.h"
#include "listitembase.h"
#include "loadingdlg.h"
#include "tcpmgr.h"
#include "usermgr.h"

#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWidget>

SearchList::SearchList(QWidget *parent)
    : QListWidget(parent)
    , _send_pending(false)
    , _search_edit(nullptr)
    , _loadingDialog(nullptr)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);

    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    addTipItem();

    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if (_find_dlg) {
        _find_dlg->close();
        _find_dlg.reset();
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
}

bool SearchList::IsClickOnFindDlg(const QPoint &globalPos) const
{
    if (!_find_dlg || !_find_dlg->isVisible()) {
        return false;
    }

    QWidget *clickedWidget = QApplication::widgetAt(globalPos);
    if (clickedWidget == _find_dlg.get() || _find_dlg->isAncestorOf(clickedWidget)) {
        return true;
    }

    const QRect dlgRect(_find_dlg->mapToGlobal(QPoint(0, 0)), _find_dlg->size());
    return dlgRect.contains(globalPos);
}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport()) {
        if (event->type() == QEvent::Enter) {
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else if (event->type() == QEvent::Wheel) {
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
            const int numDegrees = wheelEvent->angleDelta().y() / 8;
            const int numSteps = numDegrees / 15;
            verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps);
            return true;
        } else if (event->type() == QEvent::MouseButtonPress && _find_dlg && _find_dlg->isVisible()) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPosition().toPoint());
            if (clickedWidget != _find_dlg.get() && !_find_dlg->isAncestorOf(clickedWidget)) {
                CloseFindDlg();
            }
        }
    }
    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool pending)
{
    if (pending) {
        _loadingDialog = new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = true;
    } else if (_loadingDialog) {
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _loadingDialog = nullptr;
        _send_pending = false;
    }
}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    auto *item_tmp = new QListWidgetItem;
    item_tmp->setSizeHint(QSize(250, 10));
    addItem(item_tmp);
    invalid_item->setObjectName(QStringLiteral("invalid_item"));
    setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);

    auto *add_user_item = new AddUserItem();
    auto *item = new QListWidgetItem;
    item->setSizeHint(add_user_item->sizeHint());
    addItem(item);
    setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = itemWidget(item);
    if (!widget) {
        return;
    }

    auto *customItem = qobject_cast<ListItemBase *>(widget);
    if (!customItem) {
        return;
    }

    const auto itemType = customItem->GetItemType();
    if (itemType == ListItemType::INVALID_ITEM) {
        CloseFindDlg();
        return;
    }

    if (itemType == ListItemType::ADD_USER_TIP_ITEM) {
        if (_send_pending || !_search_edit) {
            return;
        }

        waitPending(true);
        const auto *search_edit = dynamic_cast<CustomizeEdit *>(_search_edit);
        if (!search_edit) {
            waitPending(false);
            return;
        }

        QJsonObject jsonObj;
        jsonObj["uid"] = search_edit->text();

        const QJsonDocument doc(jsonObj);
        const QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_SEARCH_USER_REQ, jsonData);
        return;
    }

    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);

    if (si == nullptr) {
        _find_dlg = std::make_shared<FindFailDlg>(this);
    } else {
        const auto self_uid = UserMgr::GetInstance()->GetUid();
        if (si->_uid == self_uid) {
            return;
        }

        if (UserMgr::GetInstance()->CheckFriendById(si->_uid)) {
            emit sig_jump_chat_item(si);
            return;
        }

        _find_dlg = std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg)->SetSearchInfo(si);
    }

    if (_find_dlg) {
        _find_dlg->show();
    }
}
