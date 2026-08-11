#include "contactuserlist.h"

#include "conuseritem.h"
#include "grouptipitem.h"
#include "listitembase.h"
#include "tcpmgr.h"
#include "usermgr.h"

#include <QDebug>
#include <QEvent>
#include <QListWidgetItem>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>

namespace {
const QStringList kHeads = {
    QStringLiteral(":/res/head_1.jpg"),
    QStringLiteral(":/res/head_2.jpg"),
    QStringLiteral(":/res/head_3.jpg"),
    QStringLiteral(":/res/head_4.jpg"),
    QStringLiteral(":/res/head_5.jpg"),
};

const QStringList kNames = {
    QStringLiteral("hexing"),
    QStringLiteral("zack"),
    QStringLiteral("golang"),
    QStringLiteral("cpp"),
    QStringLiteral("java"),
    QStringLiteral("nodejs"),
    QStringLiteral("python"),
    QStringLiteral("rust"),
};
}

ContactUserList::ContactUserList(QWidget *parent)
    : QListWidget(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);

    addContactUserList();
    connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);

    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ContactUserList::slot_add_auth_firend);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ContactUserList::slot_auth_rsp);
}

void ContactUserList::ShowRedPoint(bool bshow)
{
    if (_add_friend_item) {
        _add_friend_item->ShowRedPoint(bshow);
    }
}

void ContactUserList::addContactUserList()
{
    auto *groupTip = new GroupTipItem();
    auto *item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    addItem(item);
    setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    _add_friend_item = new ConUserItem();
    _add_friend_item->setObjectName(QStringLiteral("new_friend_item"));
    _add_friend_item->SetInfo(0, tr("新的朋友"), QStringLiteral(":/res/add_friend_normal.png"));
    _add_friend_item->SetItemType(ListItemType::APPLY_FRIEND_ITEM);

    auto *add_item = new QListWidgetItem;
    add_item->setSizeHint(_add_friend_item->sizeHint());
    addItem(add_item);
    setItemWidget(add_item, _add_friend_item);
    setCurrentItem(add_item);

    auto *groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("联系人"));
    _groupitem = new QListWidgetItem;
    _groupitem->setSizeHint(groupCon->sizeHint());
    addItem(_groupitem);
    setItemWidget(_groupitem, groupCon);
    _groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);

    const auto con_list = UserMgr::GetInstance()->GetConListPerPage();
    for (const auto &con_ele : con_list) {
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(con_ele->_uid, con_ele->_name, con_ele->_icon);
        auto *contact_item = new QListWidgetItem;
        contact_item->setSizeHint(con_user_wid->sizeHint());
        addItem(contact_item);
        setItemWidget(contact_item, con_user_wid);
    }

    UserMgr::GetInstance()->UpdateContactLoadedCount();

    if (con_list.empty()) {
        for (int i = 0; i < 13; ++i) {
            const int randomValue = QRandomGenerator::global()->bounded(100);
            const int head_i = randomValue % kHeads.size();
            const int name_i = randomValue % kNames.size();

            auto *con_user_wid = new ConUserItem();
            con_user_wid->SetInfo(0, kNames.at(name_i), kHeads.at(head_i));
            auto *contact_item = new QListWidgetItem;
            contact_item->setSizeHint(con_user_wid->sizeHint());
            addItem(contact_item);
            setItemWidget(contact_item, con_user_wid);
        }
    }
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
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

            if (verticalScrollBar()->maximum() - verticalScrollBar()->value() <= 0) {
                if (UserMgr::GetInstance()->IsLoadConFin() || _load_pending) {
                    return true;
                }

                _load_pending = true;
                QTimer::singleShot(100, this, [this]() {
                    _load_pending = false;
                });

                qDebug() << "load more contact user";
                emit sig_loading_contact_user();
            }
            return true;
        }
    }

    return QListWidget::eventFilter(watched, event);
}

void ContactUserList::slot_item_clicked(QListWidgetItem *item)
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
    if (itemType == ListItemType::INVALID_ITEM || itemType == ListItemType::GROUP_TIP_ITEM) {
        return;
    }

    if (itemType == ListItemType::APPLY_FRIEND_ITEM) {
        qDebug() << "apply friend item clicked";
        emit sig_switch_apply_friend_page();
        return;
    }

    if (itemType == ListItemType::CONTACT_USER_ITEM) {
        qDebug() << "contact user item clicked";
        const auto *con_item = qobject_cast<ConUserItem *>(customItem);
        if (con_item) {
            emit sig_switch_friend_info_page(con_item->GetInfo());
        }
    }
}

void ContactUserList::slot_add_auth_firend(const std::shared_ptr<AuthInfo> &auth_info)
{
    if (!auth_info) {
        return;
    }

    qDebug() << "slot add auth friend";
    if (UserMgr::GetInstance()->CheckFriendById(auth_info->_uid)) {
        return;
    }

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_info->_uid, auth_info->_name, auth_info->_icon);
    auto *new_item = new QListWidgetItem;
    new_item->setSizeHint(con_user_wid->sizeHint());

    const int index = row(_groupitem);
    insertItem(index + 1, new_item);
    setItemWidget(new_item, con_user_wid);
}

void ContactUserList::slot_auth_rsp(const std::shared_ptr<AuthRsp> &auth_rsp)
{
    if (!auth_rsp) {
        return;
    }

    qDebug() << "slot auth rsp called";
    if (UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid)) {
        return;
    }

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_rsp->_uid, auth_rsp->_name, auth_rsp->_icon);
    auto *new_item = new QListWidgetItem;
    new_item->setSizeHint(con_user_wid->sizeHint());

    const int index = row(_groupitem);
    insertItem(index + 1, new_item);
    setItemWidget(new_item, con_user_wid);
}
