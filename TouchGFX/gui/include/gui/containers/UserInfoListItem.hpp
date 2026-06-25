#ifndef USERINFOLISTITEM_HPP
#define USERINFOLISTITEM_HPP

#include <gui_generated/containers/UserInfoListItemBase.hpp>
#include "bodbody-helper.h"

class UserInfoListItem : public UserInfoListItemBase
{
public:
    UserInfoListItem();
    virtual ~UserInfoListItem() {}

    virtual void initialize();

    void setUserInfo(bodbody_user_info_t *info);
protected:
};

#endif // USERINFOLISTITEM_HPP
