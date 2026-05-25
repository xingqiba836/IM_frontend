/******************************************************************************
 *
 * @file       registerdialog.h
 * @brief      Register dialog for chat client
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include "global.h"

#include <QDialog>
#include <QJsonObject>
#include <QMap>
#include <QTimer>

#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui {
class RegisterDialog;
}
QT_END_NAMESPACE

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog() override;

signals:
    void sigSwitchLogin();

private slots:
    void on_sure_btn_clicked();
    void on_cancel_btn_clicked();
    void on_return_btn_clicked();
    void on_varify_btn_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    void showTip(const QString &text, bool ok);
    void initHttpHandlers();
    void AddTipErr(TipErr te, const QString &tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();
    void ChangeTipPage();

    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    QTimer *_countdown_timer;
    int _countdown;
};

#endif // REGISTERDIALOG_H
