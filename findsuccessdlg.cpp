#include "findsuccessdlg.h"
#include "applyfriend.h"
#include "clickedbtn.h"
#include "ui_findsuccessdlg.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QPixmap>

FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDlg)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("添加"));
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setModal(false);

    ui->add_friend_btn->SetState(QStringLiteral("normal"),
                                 QStringLiteral("hover"),
                                 QStringLiteral("press"));
    ui->close_btn->SetState(QStringLiteral("normal"),
                            QStringLiteral("hover"),
                            QStringLiteral("press"));

    connect(ui->close_btn, &ClickedBtn::clicked, this, &FindSuccessDlg::slot_close_btn_clicked);
    connect(ui->add_friend_btn, &ClickedBtn::clicked, this, &FindSuccessDlg::slot_add_friend_btn_clicked);

    QString pix_path = QDir::toNativeSeparators(
        QCoreApplication::applicationDirPath()
        + QDir::separator() + QStringLiteral("static")
        + QDir::separator() + QStringLiteral("head_1.jpg"));
    if (!QFile::exists(pix_path)) {
        pix_path = QStringLiteral(":/res/head_1.jpg");
    }

    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
}

FindSuccessDlg::~FindSuccessDlg()
{
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(const std::shared_ptr<SearchInfo> &si)
{
    _si = si;
    if (!si) {
        return;
    }
    ui->name_lb->setText(si->_name);
}

void FindSuccessDlg::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
        return;
    }
    QDialog::keyPressEvent(event);
}

void FindSuccessDlg::slot_add_friend_btn_clicked()
{
    if (!_si) {
        return;
    }

    auto si = _si;
    QWidget *host = window();
    if (!host) {
        host = parentWidget();
    }

    hide();

    ApplyFriend apply_dlg(host);
    apply_dlg.SetSearchInfo(si);

    const QRect hostRect = host->frameGeometry();
    apply_dlg.move(hostRect.center()
                   - QPoint(apply_dlg.width() / 2, apply_dlg.height() / 2));
    apply_dlg.exec();

    close();
}

void FindSuccessDlg::slot_close_btn_clicked()
{
    close();
}
