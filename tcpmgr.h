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
#include "userdata.h"

#include <QByteArray>
#include <QJsonArray>
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
    ~TcpMgr();
    void CloseConnection();

public slots:
    void slot_tcp_connect(ServerInfo si);
    void slot_send_data(ReqId reqId, QByteArray data);

signals:
    void sig_con_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QByteArray data);
    void sig_swich_chatdlg();
    void sig_load_apply_list(QJsonArray json_array);
    void sig_login_failed(int err);
    void sig_user_search(std::shared_ptr<SearchInfo> si);
    void sig_friend_apply(std::shared_ptr<AddFriendApply> apply);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void sig_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    void sig_text_chat_msg(std::vector<std::shared_ptr<TextChatData>> msg_list);
    void sig_notify_offline();
    void sig_connection_closed();
    void sig_load_chat_thread(bool load_more, int last_thread_id,
                              std::vector<std::shared_ptr<ChatThreadInfo>> chat_list);
    void sig_create_private_chat(int uid, int other_id, int thread_id);
    void sig_load_chat_msg(int thread_id, int message_id, bool load_more,
                           std::vector<std::shared_ptr<TextChatData>> msg_list);
    void sig_chat_msg_rsp(int thread_id, std::vector<std::shared_ptr<TextChatData>> msg_list);

private:
    friend class Singleton<TcpMgr>;

    TcpMgr();
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
