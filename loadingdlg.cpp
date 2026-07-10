/******************************************************************************
 *
 * @file       loadingdlg.cpp
 * @brief      Loading dialog
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "loadingdlg.h"
#include "ui_loadingdlg.h"

LoadingDlg::LoadingDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);
}

LoadingDlg::~LoadingDlg()
{
    delete ui;
}
