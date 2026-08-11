#include "userinfopage.h"
#include "ui_userinfopage.h"
#include "usermgr.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QPixmap>

UserInfoPage::UserInfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserInfoPage)
{
    ui->setupUi(this);

    QString pix_path = QDir::toNativeSeparators(
        QCoreApplication::applicationDirPath()
        + QDir::separator() + QStringLiteral("static")
        + QDir::separator() + QStringLiteral("head_1.jpg"));
    if (!QFile::exists(pix_path)) {
        pix_path = QStringLiteral(":/res/head_1.jpg");
    }

    QPixmap pixmap(pix_path);
    ui->head_lb->setPixmap(pixmap.scaled(ui->head_lb->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));
    ui->head_lb->setScaledContents(true);

    const QString name = UserMgr::GetInstance()->GetName();
    ui->nick_ed->setText(name);
    ui->name_ed->setText(name);
    ui->desc_ed->clear();

    connect(ui->up_btn, &QPushButton::clicked, this, &UserInfoPage::slot_up_btn_clicked);
    connect(ui->submit_btn, &QPushButton::clicked, this, &UserInfoPage::slot_submit_btn_clicked);
}

UserInfoPage::~UserInfoPage()
{
    delete ui;
}

void UserInfoPage::slot_up_btn_clicked()
{
    const QString filename = QFileDialog::getOpenFileName(
        this,
        tr("选择图片"),
        QString(),
        tr("图片文件 (*.png *.jpg *.jpeg *.bmp *.webp)"));
    if (filename.isEmpty()) {
        return;
    }

    QPixmap inputImage;
    if (!inputImage.load(filename)) {
        qDebug() << "Failed to load image:" << filename;
        return;
    }

    const QPixmap scaledPixmap = inputImage.scaled(ui->head_lb->size(),
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
    ui->head_lb->setPixmap(scaledPixmap);
    ui->head_lb->setScaledContents(true);
}

void UserInfoPage::slot_submit_btn_clicked()
{
    qDebug() << "user info submit clicked";
}
