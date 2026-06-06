/******************************************************************************
 *
 * @file       tcpmgr.cpp
 * @brief      TCP connection manager implementation
 *
 * @author     hexing
 * @date       2026/05/25
 *
 *****************************************************************************/
#include "tcpmgr.h"

#include <QAbstractSocket>
#include <QDataStream>
#include <QDebug>
#include <QIODevice>

TcpMgr::TcpMgr()
    : _host(QString())
    , _port(0)
    , _b_recv_pending(false)
    , _message_id(0)
    , _message_len(0)
{
    connect(&_socket, &QTcpSocket::connected, this, [this]() {
        qDebug() << "Connected to server!";
        emit sig_con_success(true);
    });

    connect(&_socket, &QTcpSocket::readyRead, this, [this]() {
        _buffer.append(_socket.readAll());

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);

        for (;;) {
            if (!_b_recv_pending) {
                if (_buffer.size() < static_cast<int>(sizeof(quint16) * 2)) {
                    return;
                }

                stream >> _message_id >> _message_len;
                _buffer = _buffer.mid(sizeof(quint16) * 2);

                qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
            }

            if (_buffer.size() < _message_len) {
                _b_recv_pending = true;
                return;
            }

            _b_recv_pending = false;
            const QByteArray messageBody = _buffer.mid(0, _message_len);
            qDebug() << "receive body msg is " << messageBody;

            _buffer = _buffer.mid(_message_len);
        }
    });

    connect(&_socket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError) {
        qDebug() << "Error:" << _socket.errorString();
        switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            qDebug() << "Connection Refused!";
            emit sig_con_success(false);
            break;
        case QAbstractSocket::RemoteHostClosedError:
            qDebug() << "Remote Host Closed Connection!";
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug() << "Host Not Found!";
            emit sig_con_success(false);
            break;
        case QAbstractSocket::SocketTimeoutError:
            qDebug() << "Connection Timeout!";
            emit sig_con_success(false);
            break;
        case QAbstractSocket::NetworkError:
            qDebug() << "Network Error!";
            break;
        default:
            qDebug() << "Other Error!";
            break;
        }
    });

    connect(&_socket, &QTcpSocket::disconnected, this, []() {
        qDebug() << "Disconnected from server.";
    });

    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug() << "receive tcp connect signal";
    qDebug() << "Connecting to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QString data)
{
    const quint16 id = static_cast<quint16>(reqId);
    const quint16 len = static_cast<quint16>(data.size());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << id << len;
    block.append(data.toUtf8());

    _socket.write(block);
}
