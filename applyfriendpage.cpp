#include "applyfriendpage.h"
#include "applyfrienditem.h"
#include "applyfriendlist.h"
#include "authenfriend.h"
#include "tcpmgr.h"
#include "ui_applyfriendpage.h"
#include "usermgr.h"

#include <QListWidgetItem>
#include <QPainter>
#include <QRandomGenerator>
#include <QStyle>
#include <QStyleOption>

namespace {
const QStringList kMsgs = {
    QStringLiteral("hello world !"),
    QStringLiteral("nice to meet u"),
    QStringLiteral("New year, new life"),
    QStringLiteral("You have to love yourself"),
    QStringLiteral("My love is written in the wind"),
};

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

void connectAuthFriend(ApplyFriendPage *page, ApplyFriendItem *apply_item)
{
    QObject::connect(apply_item, &ApplyFriendItem::sig_auth_friend, page,
                     [page](const std::shared_ptr<ApplyInfo> &apply_info) {
                         auto *authFriend = new AuthenFriend(page);
                         authFriend->setModal(true);
                         authFriend->SetApplyInfo(apply_info);
                         authFriend->show();
                     });
}
}

ApplyFriendPage::ApplyFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplyFriendPage)
{
    ui->setupUi(this);
    connect(ui->apply_friend_list, &ApplyFriendList::sig_show_search,
            this, &ApplyFriendPage::sig_show_search);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ApplyFriendPage::slot_auth_rsp);
    loadApplyList();
}

ApplyFriendPage::~ApplyFriendPage()
{
    delete ui;
}

void ApplyFriendPage::AddNewApply(const std::shared_ptr<AddFriendApply> &apply)
{
    if (!apply) {
        return;
    }

    const int randomValue = QRandomGenerator::global()->bounded(100);
    const int head_i = randomValue % kHeads.size();
    auto *apply_item = new ApplyFriendItem();
    const auto apply_info = std::make_shared<ApplyInfo>(apply->_from_uid,
                                                        apply->_name,
                                                        apply->_desc,
                                                        kHeads.at(head_i),
                                                        apply->_nick,
                                                        apply->_sex,
                                                        0);
    apply_item->SetInfo(apply_info);
    auto *item = new QListWidgetItem;
    item->setSizeHint(apply_item->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    ui->apply_friend_list->insertItem(0, item);
    ui->apply_friend_list->setItemWidget(item, apply_item);
    apply_item->ShowAddBtn(true);
    _unauth_items[apply_item->GetUid()] = apply_item;

    connectAuthFriend(this, apply_item);
}

void ApplyFriendPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ApplyFriendPage::loadApplyList()
{
    const auto apply_list = UserMgr::GetInstance()->GetApplyList();
    for (auto &apply : apply_list) {
        if (!apply) {
            continue;
        }

        const int randomValue = QRandomGenerator::global()->bounded(100);
        const int head_i = randomValue % kHeads.size();
        auto *apply_item = new ApplyFriendItem();
        apply->SetIcon(kHeads.at(head_i));
        apply_item->SetInfo(apply);
        auto *item = new QListWidgetItem;
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->insertItem(0, item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        if (apply->_status) {
            apply_item->ShowAddBtn(false);
        } else {
            apply_item->ShowAddBtn(true);
            _unauth_items[apply_item->GetUid()] = apply_item;
        }

        connectAuthFriend(this, apply_item);
    }

    for (int i = 0; i < 13; ++i) {
        const int randomValue = QRandomGenerator::global()->bounded(100);
        const int str_i = randomValue % kMsgs.size();
        const int head_i = randomValue % kHeads.size();
        const int name_i = randomValue % kNames.size();

        auto *apply_item = new ApplyFriendItem();
        const auto apply = std::make_shared<ApplyInfo>(0,
                                                     kNames.at(name_i),
                                                     kMsgs.at(str_i),
                                                     kHeads.at(head_i),
                                                     kNames.at(name_i),
                                                     0,
                                                     1);
        apply_item->SetInfo(apply);
        auto *item = new QListWidgetItem;
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->addItem(item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        apply_item->ShowAddBtn(false);

        connectAuthFriend(this, apply_item);
    }
}

void ApplyFriendPage::slot_auth_rsp(const std::shared_ptr<AuthRsp> &auth_rsp)
{
    if (!auth_rsp) {
        return;
    }

    const auto find_iter = _unauth_items.find(auth_rsp->_uid);
    if (find_iter == _unauth_items.end()) {
        return;
    }

    find_iter->second->ShowAddBtn(false);
}
