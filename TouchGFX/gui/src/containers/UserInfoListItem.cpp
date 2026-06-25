#include <gui/containers/UserInfoListItem.hpp>

UserInfoListItem::UserInfoListItem() {}

void UserInfoListItem::initialize() { UserInfoListItemBase::initialize(); }

void UserInfoListItem::setUserInfo(bodbody_user_info_t *info) {
  char buf[32];
  // Unicode::snprintf(buf, 32, "%d", info->age);
  // age->setTypedText(TypedText(T_AGE));
  // age->setText(buf);

  // Unicode::snprintf(buf, 32, "%d", info->height);
  // height->setTypedText(TypedText(T_HEIGHT));
}
