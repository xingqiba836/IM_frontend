/******************************************************************************
 *
 * @file       httpmgr.cpp
 * @brief      HTTP request manager implementation
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "httpmgr.h"

#include <QDebug>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

HttpMgr::~HttpMgr() = default;

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    const QByteArray data = QJsonDocument(json).toJson();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    auto self = shared_from_this();
    QNetworkReply *reply = _manager.post(request, data);

    connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            emit self->sig_http_finish(req_id, "", ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        const QString res = QString::fromUtf8(reply->readAll());
        emit self->sig_http_finish(req_id, res, SUCCESS, mod);
        reply->deleteLater();
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if (mod == REGISTERMOD) {
        emit sig_reg_mod_finish(id, res, err);
    }
}
