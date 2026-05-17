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

extern std::function<void(QWidget *)> repolish;
extern QString gate_url_prefix;

enum ReqId {
    ID_GET_VARIFY_CODE = 1001,
    ID_REG_USER = 1002,
};

enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1,
    ERR_NETWORK = 2,
};

enum Modules {
    REGISTERMOD = 0,
};

#endif // GLOBAL_H
