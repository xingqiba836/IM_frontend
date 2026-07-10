/******************************************************************************
 *
 * @file       usermgr.h
 * @brief      User data manager (singleton)
 *
 * @author     hexing
 * @date       2026/05/25
 *
 *****************************************************************************/
#ifndef USERMGR_H
#define USERMGR_H

#include "singleton.h"

#include <QObject>
#include <QString>

#include <memory>

class UserMgr : public QObject, public Singleton<UserMgr>, public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT

public:
    friend class Singleton<UserMgr>;
    ~UserMgr() override;

    void SetName(const QString &name);
    void SetUid(int uid);
    void SetToken(const QString &token);

private:
    UserMgr();

    QString _name;
    QString _token;
    int _uid = 0;
};

#endif // USERMGR_H
