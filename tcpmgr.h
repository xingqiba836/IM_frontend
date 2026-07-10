/******************************************************************************
 *
 * @file       tcpmgr.h
 * @brief      TCP connection manager (singleton)
 *
 * @author     hexing
 * @date       2026/05/25
 *
 *****************************************************************************/
#ifndef TCPMGR_H
#define TCPMGR_H

#include "global.h"
#include "singleton.h"

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QTcpSocket>

#include <cstdint>
#include <functional>
#include <memory>

class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT

public:
    TcpMgr();

public slots:
    void slot_tcp_connect(ServerInfo si);
    void slot_send_data(ReqId reqId, QString data);

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QString data);
    void sig_login_failed(int err);
    void sig_swich_chatdlg();

private:
    friend class Singleton<TcpMgr>;

    void initHandlers();
    void handleMsg(ReqId id, int len, const QByteArray &data);

    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
    QMap<ReqId, std::function<void(ReqId, int, QByteArray)>> _handlers;
};

#endif // TCPMGR_H
