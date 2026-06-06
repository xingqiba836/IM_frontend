/******************************************************************************
 *
 * @file       resetdialog.h
 * @brief      Reset password dialog for chat client
 *
 * @author     hexing
 * @date       2026/05/25
 *
 *****************************************************************************/
#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include "global.h"

#include <QDialog>
#include <QJsonObject>
#include <QMap>

#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui {
class ResetDialog;
}
QT_END_NAMESPACE

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog() override;

signals:
    void switchLogin();

private slots:
    void on_sure_btn_clicked();
    void on_cancel_btn_clicked();
    void on_varify_btn_clicked();
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    void showTip(const QString &text, bool ok);
    void initHandlers();
    void AddTipErr(TipErr te, const QString &tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVarifyValid();

    Ui::ResetDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
};

#endif // RESETDIALOG_H
