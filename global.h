/******************************************************************************
 *
 * @file       global.h
 * @brief      Global enums and utilities
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QPixmap>
#include <QVector>
#include <QWidget>
#include <functional>

class QWidget;

struct ServerInfo {
    QString Host;
    QString Port;
    QString Token;
    int Uid = 0;
};

extern std::function<void(QWidget *)> repolish;
extern QString gate_url_prefix;

enum ReqId {
    ID_GET_VARIFY_CODE = 1001,
    ID_REG_USER = 1002,
    ID_RESET_PWD = 1003,
    ID_LOGIN_USER = 1004,
    ID_CHAT_LOGIN = 1005,
    ID_CHAT_LOGIN_RSP = 1006,
    ID_SEARCH_USER_REQ = 1007,
    ID_SEARCH_USER_RSP = 1008,
    ID_ADD_FRIEND_REQ = 1009,
    ID_ADD_FRIEND_RSP = 1010,
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,
    ID_AUTH_FRIEND_REQ = 1013,
    ID_AUTH_FRIEND_RSP = 1014,
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015,
    ID_TEXT_CHAT_MSG_REQ = 1017,
    ID_TEXT_CHAT_MSG_RSP = 1018,
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,
    ID_NOTIFY_OFF_LINE_REQ = 1021,
    ID_HEART_BEAT_REQ = 1023,
    ID_HEARTBEAT_RSP = 1024,
    ID_LOAD_CHAT_THREAD_REQ = 1025,
    ID_LOAD_CHAT_THREAD_RSP = 1026,
    ID_CREATE_PRIVATE_CHAT_REQ = 1027,
    ID_CREATE_PRIVATE_CHAT_RSP = 1028,
    ID_LOAD_CHAT_MSG_REQ = 1029,
    ID_LOAD_CHAT_MSG_RSP = 1030,
};

enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1,
    ERR_NETWORK = 2,
};

// GateServer 业务错误码（与 const.h 保持一致）
enum ServerError {
    SERVER_ERR_JSON = 1001,
    SERVER_RPC_FAILED = 1002,
    SERVER_VARIFY_EXPIRED = 1003,
    SERVER_VARIFY_CODE_ERR = 1004,
    SERVER_USER_EXIST = 1005,
    SERVER_PASSWD_ERR = 1006,
    SERVER_EMAIL_NOT_MATCH = 1007,
    SERVER_PASSWD_UP_FAILED = 1008,
    SERVER_PASSWD_INVALID = 1009,
    SERVER_RPC_GET_FAILED = 1010,
};

QString serverErrorTip(int error);
QString xorString(const QString &input);

enum Modules {
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2,
};

enum TipErr {
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6,
};

enum ClickLbState {
    Normal = 0,
    Selected = 1,
};

enum ListItemType {
    CHAT_USER_ITEM = 0,
    CONTACT_USER_ITEM = 1,
    SEARCH_USER_ITEM = 2,
    ADD_USER_TIP_ITEM = 3,
    INVALID_ITEM = 4,
    GROUP_TIP_ITEM = 5,
    LINE_ITEM = 6,
    APPLY_FRIEND_ITEM = 7,
};

enum class ChatUIMode {
    ChatMode = 0,
    ContactMode = 1,
    SearchMode = 2,
    SettingsMode = 3,
};

enum class ChatRole {
    Self,
    Other,
};

struct MsgInfo {
    QString msgFlag;
    QString content;
    QPixmap pixmap;
};

const int MIN_APPLY_LABEL_ED_LEN = 40;
const QString add_prefix = QStringLiteral("添加标签 ");
const int tip_offset = 5;

const int CHAT_COUNT_PER_PAGE = 13;

enum MsgStatus {
    UN_READ = 0,
    SEND_FAILED = 1,
    READED = 2,
};

enum class ChatFormType {
    PRIVATE = 0,
    GROUP = 1,
};

enum class ChatMsgType {
    TEXT = 0,
    PIC = 1,
    FILE = 2,
};

#endif // GLOBAL_H
