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

#endif // GLOBAL_H
