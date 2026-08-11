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
#include "usermgr.h"

#include <QAbstractSocket>
#include <QDataStream>
#include <QDebug>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>

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
            handleMsg(static_cast<ReqId>(_message_id), _message_len, messageBody);
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

    connect(&_socket, &QTcpSocket::disconnected, this, [this]() {
        qDebug() << "Disconnected from server.";
        emit sig_connection_closed();
    });

    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    initHandlers();
}

TcpMgr::~TcpMgr() = default;

void TcpMgr::CloseConnection()
{
    _socket.close();
}

void TcpMgr::initHandlers()
{
    _handlers.insert(ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        qDebug() << "data jsonobj is " << jsonObj;

        if (!jsonObj.contains("error")) {
            const int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err;
            emit sig_login_failed(err);
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Login Failed, err is " << err;
            emit sig_login_failed(err);
            return;
        }

        const auto uid = jsonObj["uid"].toInt();
        const auto name = jsonObj["name"].toString();
        const auto nick = jsonObj["nick"].toString();
        const auto icon = jsonObj["icon"].toString();
        const auto sex = jsonObj["sex"].toInt();
        const auto desc = jsonObj["desc"].toString();
        const auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex, QString(), desc);

        UserMgr::GetInstance()->SetUserInfo(user_info);
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());
        if (jsonObj.contains("apply_list")) {
            UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
        }

        if (jsonObj.contains("friend_list")) {
            UserMgr::GetInstance()->AppendFriendList(jsonObj["friend_list"].toArray());
        }

        emit sig_swich_chatdlg();
    });

    _handlers.insert(ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            emit sig_user_search(nullptr);
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            emit sig_user_search(nullptr);
            return;
        }

        const auto search_info = std::make_shared<SearchInfo>(jsonObj["uid"].toInt(),
                                                              jsonObj["name"].toString(),
                                                              jsonObj["nick"].toString(),
                                                              jsonObj["desc"].toString(),
                                                              jsonObj["sex"].toInt(),
                                                              jsonObj["icon"].toString());
        emit sig_user_search(search_info);
    });

    _handlers.insert(ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            emit sig_user_search(nullptr);
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            emit sig_user_search(nullptr);
            return;
        }

        const int from_uid = jsonObj["applyuid"].toInt();
        const QString name = jsonObj["name"].toString();
        const QString desc = jsonObj["desc"].toString();
        const QString icon = jsonObj["icon"].toString();
        const QString nick = jsonObj["nick"].toString();
        const int sex = jsonObj["sex"].toInt();

        const auto apply_info = std::make_shared<AddFriendApply>(from_uid, name, desc, icon, nick, sex);
        emit sig_friend_apply(apply_info);
    });

    _handlers.insert(ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            qDebug() << "Auth Friend Failed, err is Json Parse Err";
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        const int from_uid = jsonObj["fromuid"].toInt();
        const QString name = jsonObj["name"].toString();
        const QString nick = jsonObj["nick"].toString();
        const QString icon = jsonObj["icon"].toString();
        const int sex = jsonObj["sex"].toInt();

        std::vector<std::shared_ptr<TextChatData>> chat_datas;
        for (const QJsonValue &value : jsonObj["chat_datas"].toArray()) {
            const auto send_uid = value["sender"].toInt();
            const auto msg_id = value["msg_id"].toInt();
            const auto thread_id = value["thread_id"].toInt();
            const auto msg_content = value["msg_content"].toString();
            const QString chat_time = value["chat_time"].toString();
            const auto status = value["status"].toInt();
            const auto chat_data = std::make_shared<TextChatData>(msg_id,
                                                                  thread_id,
                                                                  ChatFormType::PRIVATE,
                                                                  ChatMsgType::TEXT,
                                                                  msg_content,
                                                                  send_uid,
                                                                  status,
                                                                  chat_time);
            chat_datas.push_back(chat_data);
        }

        const auto auth_info = std::make_shared<AuthInfo>(from_uid, name, nick, icon, sex);
        auth_info->SetChatDatas(chat_datas);
        emit sig_add_auth_friend(auth_info);
    });

    _handlers.insert(ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        Q_UNUSED(id);
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Add Friend Failed, err is " << err;
            return;
        }

        qDebug() << "Add Friend Success";
    });

    _handlers.insert(ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            qDebug() << "Auth Friend Failed, err is Json Parse Err";
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        const auto name = jsonObj["name"].toString();
        const auto nick = jsonObj["nick"].toString();
        const auto icon = jsonObj["icon"].toString();
        const auto sex = jsonObj["sex"].toInt();
        const auto uid = jsonObj["uid"].toInt();

        std::vector<std::shared_ptr<TextChatData>> chat_datas;
        for (const QJsonValue &value : jsonObj["chat_datas"].toArray()) {
            const auto send_uid = value["sender"].toInt();
            const auto msg_id = value["msg_id"].toInt();
            const auto thread_id = value["thread_id"].toInt();
            const auto msg_content = value["msg_content"].toString();
            const auto status = value["status"].toInt();
            const auto chat_data = std::make_shared<TextChatData>(msg_id,
                                                                  thread_id,
                                                                  ChatFormType::PRIVATE,
                                                                  ChatMsgType::TEXT,
                                                                  msg_content,
                                                                  send_uid,
                                                                  status);
            chat_datas.push_back(chat_data);
        }

        const auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        rsp->SetChatDatas(chat_datas);
        emit sig_auth_rsp(rsp);

        qDebug() << "Auth Friend Success";
    });

    _handlers.insert(ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err";
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success";

        const auto thread_id = jsonObj["thread_id"].toInt();
        const auto sender = jsonObj["fromuid"].toInt();

        std::vector<std::shared_ptr<TextChatData>> chat_datas;
        for (const QJsonValue &value : jsonObj["chat_datas"].toArray()) {
            const auto msg_id = value["message_id"].toInt();
            const auto unique_id = value["unique_id"].toString();
            const auto msg_content = value["content"].toString();
            const QString chat_time = value["chat_time"].toString();
            const int status = value["status"].toInt();
            const auto chat_data = std::make_shared<TextChatData>(msg_id,
                                                                  unique_id,
                                                                  thread_id,
                                                                  ChatFormType::PRIVATE,
                                                                  ChatMsgType::TEXT,
                                                                  msg_content,
                                                                  sender,
                                                                  status,
                                                                  chat_time);
            chat_datas.push_back(chat_data);
        }

        emit sig_chat_msg_rsp(thread_id, chat_datas);
    });

    _handlers.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err";
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Notify Success";

        const auto thread_id = jsonObj["thread_id"].toInt();
        const auto sender = jsonObj["fromuid"].toInt();

        std::vector<std::shared_ptr<TextChatData>> chat_datas;
        for (const QJsonValue &value : jsonObj["chat_datas"].toArray()) {
            const auto msg_id = value["message_id"].toInt();
            const auto unique_id = value["unique_id"].toString();
            const auto msg_content = value["content"].toString();
            const QString chat_time = value["chat_time"].toString();
            const int status = value["status"].toInt();
            const auto chat_data = std::make_shared<TextChatData>(msg_id,
                                                                  unique_id,
                                                                  thread_id,
                                                                  ChatFormType::PRIVATE,
                                                                  ChatMsgType::TEXT,
                                                                  msg_content,
                                                                  sender,
                                                                  status,
                                                                  chat_time);
            chat_datas.push_back(chat_data);
        }

        emit sig_text_chat_msg(chat_datas);
    });

    _handlers.insert(ID_NOTIFY_OFF_LINE_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        const auto uid = jsonObj["uid"].toInt();
        qDebug() << "Receive offline Notify Success, uid is " << uid;
        emit sig_notify_offline();
    });

    _handlers.insert(ID_HEARTBEAT_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        Q_UNUSED(id);
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        qDebug() << "Receive Heart Beat Msg Success";
    });

    _handlers.insert(ID_LOAD_CHAT_THREAD_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        const auto thread_array = jsonObj["threads"].toArray();
        std::vector<std::shared_ptr<ChatThreadInfo>> chat_threads;
        for (const QJsonValue &value : thread_array) {
            const auto cti = std::make_shared<ChatThreadInfo>();
            cti->_thread_id = value["thread_id"].toInt();
            cti->_type = value["type"].toString();
            cti->_user1_id = value["user1_id"].toInt();
            cti->_user2_id = value["user2_id"].toInt();
            chat_threads.push_back(cti);
        }

        const bool load_more = jsonObj["load_more"].toBool();
        const int next_last_id = jsonObj["next_last_id"].toInt();
        emit sig_load_chat_thread(load_more, next_last_id, chat_threads);
    });

    _handlers.insert(ID_CREATE_PRIVATE_CHAT_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        const int uid = jsonObj["uid"].toInt();
        const int other_id = jsonObj["other_id"].toInt();
        const int thread_id = jsonObj["thread_id"].toInt();
        emit sig_create_private_chat(uid, other_id, thread_id);
    });

    _handlers.insert(ID_LOAD_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;

        const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) {
            return;
        }

        const QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            return;
        }

        const int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            return;
        }

        const int thread_id = jsonObj["thread_id"].toInt();
        const int last_msg_id = jsonObj["last_message_id"].toInt();
        const bool load_more = jsonObj["load_more"].toBool();

        std::vector<std::shared_ptr<TextChatData>> chat_datas;
        for (const QJsonValue &value : jsonObj["chat_datas"].toArray()) {
            const auto send_uid = value["sender"].toInt();
            const auto msg_id = value["msg_id"].toInt();
            const auto msg_thread_id = value["thread_id"].toInt();
            const auto msg_content = value["msg_content"].toString();
            const QString chat_time = value["chat_time"].toString();
            const int status = value["status"].toInt();
            const auto chat_data = std::make_shared<TextChatData>(msg_id,
                                                                  msg_thread_id,
                                                                  ChatFormType::PRIVATE,
                                                                  ChatMsgType::TEXT,
                                                                  msg_content,
                                                                  send_uid,
                                                                  status,
                                                                  chat_time);
            chat_datas.push_back(chat_data);
        }

        emit sig_load_chat_msg(thread_id, last_msg_id, load_more, chat_datas);
    });
}

void TcpMgr::handleMsg(ReqId id, int len, const QByteArray &data)
{
    const auto find_iter = _handlers.find(id);
    if (find_iter == _handlers.end()) {
        qDebug() << "not found id [" << id << "] to handle";
        return;
    }

    find_iter.value()(id, len, data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug() << "receive tcp connect signal";
    qDebug() << "Connecting to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QByteArray dataBytes)
{
    const quint16 id = static_cast<quint16>(reqId);
    const quint16 len = static_cast<quint16>(dataBytes.length());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << id << len;
    block.append(dataBytes);

    _socket.write(block);
    qDebug() << "tcp mgr send byte data is " << block;
}
