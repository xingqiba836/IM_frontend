/******************************************************************************
 *
 * @file       global.cpp
 * @brief      Global utilities implementation
 *
 * @author     hexing
 * @date       2026/05/17
 *
 *****************************************************************************/
#include "global.h"

#include <QCoreApplication>
#include <QStyle>

std::function<void(QWidget *)> repolish = [](QWidget *w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QString gate_url_prefix = QString();

QString xorString(const QString &input)
{
    QString result;
    result.reserve(input.size());
    for (int i = 0; i < input.size(); ++i) {
        result.append(QChar(input.at(i).unicode() ^ 12));
    }
    return result;
}

QString serverErrorTip(int error)
{
    switch (error) {
    case SERVER_ERR_JSON:
        return QCoreApplication::translate("Global", "json解析错误");
    case SERVER_RPC_FAILED:
        return QCoreApplication::translate("Global", "RPC请求失败");
    case SERVER_VARIFY_EXPIRED:
        return QCoreApplication::translate("Global", "验证码已过期");
    case SERVER_VARIFY_CODE_ERR:
        return QCoreApplication::translate("Global", "验证码错误");
    case SERVER_USER_EXIST:
        return QCoreApplication::translate("Global", "用户已存在");
    case SERVER_PASSWD_ERR:
        return QCoreApplication::translate("Global", "密码格式错误");
    case SERVER_EMAIL_NOT_MATCH:
        return QCoreApplication::translate("Global", "邮箱与用户名不匹配");
    case SERVER_PASSWD_UP_FAILED:
        return QCoreApplication::translate("Global", "密码更新失败");
    case SERVER_PASSWD_INVALID:
        return QCoreApplication::translate("Global", "用户名或密码错误");
    case SERVER_RPC_GET_FAILED:
        return QCoreApplication::translate("Global", "获取聊天服务器失败");
    default:
        return QCoreApplication::translate("Global", "请求失败(%1)").arg(error);
    }
}
