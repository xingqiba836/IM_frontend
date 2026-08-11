#include "findfaildlg.h"
#include "clickedbtn.h"
#include "ui_findfaildlg.h"

#include <QDebug>
#include <QKeyEvent>

FindFailDlg::FindFailDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindFailDlg)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("添加"));
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setModal(false);
    setObjectName(QStringLiteral("FindFailDlg"));

    ui->fail_sure_btn->SetState(QStringLiteral("normal"),
                                QStringLiteral("hover"),
                                QStringLiteral("press"));
    connect(ui->fail_sure_btn, &ClickedBtn::clicked, this, &FindFailDlg::slot_fail_sure_btn_clicked);
}

FindFailDlg::~FindFailDlg()
{
    qDebug() << "Find FailDlg destruct";
    delete ui;
}

void FindFailDlg::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
        return;
    }
    QDialog::keyPressEvent(event);
}

void FindFailDlg::slot_fail_sure_btn_clicked()
{
    close();
}
