/******************************************************************************
 *
 * @file       httpmgr.h
 * @brief      HTTP request manager (singleton)
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "global.h"
#include "singleton.h"

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QUrl>

#include <memory>

class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr();

    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);

private:
    friend class Singleton<HttpMgr>;
    HttpMgr();

    QNetworkAccessManager _manager;
};

#endif // HTTPMGR_H
