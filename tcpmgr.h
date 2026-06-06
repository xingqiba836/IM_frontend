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

#include <QObject>
#include <QTcpSocket>

#include <cstdint>
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

private:
    friend class Singleton<TcpMgr>;

    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;
};

#endif // TCPMGR_H
