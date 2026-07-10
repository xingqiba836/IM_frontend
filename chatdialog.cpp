/******************************************************************************
 *
 * @file       chatdialog.cpp
 * @brief      Main chat interface
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "chatdialog.h"
#include "chatuserlist.h"
#include "chatuserwid.h"
#include "loadingdlg.h"
#include "ui_chatdialog.h"

#include <QAction>
#include <QIcon>
#include <QListWidgetItem>
#include <QRandomGenerator>

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
}

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog)
    , _mode(ChatUIMode::ChatMode)
    , _state(ChatUIMode::ChatMode)
    , _b_loading(false)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(QStringLiteral(":/res/wechat.png")));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(true);

    ui->add_btn->SetState(QStringLiteral("normal"),
                          QStringLiteral("hover"),
                          QStringLiteral("press"));

    setupSearchEdit();
    ShowSearch(false);
    addChatUserList();

    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user,
            this, &ChatDialog::slot_loading_chat_user);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::setupSearchEdit()
{
    auto *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(QStringLiteral(":/res/search.png")));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    auto *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(QStringLiteral(":/res/close_transparent.png")));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(QStringLiteral(":/res/close_search.png")));
        } else {
            clearAction->setIcon(QIcon(QStringLiteral(":/res/close_transparent.png")));
        }
    });

    connect(clearAction, &QAction::triggered, this, [this, clearAction]() {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(QStringLiteral(":/res/close_transparent.png")));
        ui->search_edit->clearFocus();
        ShowSearch(false);
    });

    ui->search_edit->SetMaxLength(15);
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if (bsearch) {
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    } else if (_state == ChatUIMode::ChatMode) {
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
    } else if (_state == ChatUIMode::ContactMode) {
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
    }
}

void ChatDialog::addChatUserList()
{
    for (int i = 0; i < 13; ++i) {
        const int randomValue = QRandomGenerator::global()->bounded(100);
        const int str_i = randomValue % kMsgs.size();
        const int head_i = randomValue % kHeads.size();
        const int name_i = randomValue % kNames.size();

        auto *chat_user_wid = new ChatUserWid();
        chat_user_wid->SetInfo(kNames.at(name_i), kHeads.at(head_i), kMsgs.at(str_i));

        auto *item = new QListWidgetItem;
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::slot_loading_chat_user()
{
    if (_b_loading) {
        return;
    }

    _b_loading = true;
    auto *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    addChatUserList();
    loadingDialog->deleteLater();
    _b_loading = false;
}
