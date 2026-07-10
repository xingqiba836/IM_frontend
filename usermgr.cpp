/******************************************************************************
 *
 * @file       usermgr.cpp
 * @brief      User data manager implementation
 *
 * @author     hexing
 * @date       2026/05/25
 *
 *****************************************************************************/
#include "usermgr.h"

UserMgr::~UserMgr() = default;

void UserMgr::SetName(const QString &name)
{
    _name = name;
}

void UserMgr::SetUid(int uid)
{
    _uid = uid;
}

void UserMgr::SetToken(const QString &token)
{
    _token = token;
}

UserMgr::UserMgr() = default;
