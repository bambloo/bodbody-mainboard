#include "gui_generated/containers/UserInfoListItemBase.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/containers/Container.hpp"
#include "utils.h"
#include <gui/containers/UserInfoListItem.hpp>
#include <stdio.h>

UserInfoListItem::UserInfoListItem() {}

void UserInfoListItem::initialize() { UserInfoListItemBase::initialize(); }

void UserInfoListItem::setUserInfo(bodbody_user_info_t *info) {
  Unicode::snprintf(ageAreaBuffer, AGEAREA_SIZE, "%d", info->age);
  Unicode::fromUTF8((uint8_t *)info->name, userAreaBuffer, USERAREA_SIZE);
  Unicode::snprintfFloat(weightAreaBuffer, WEIGHTAREA_SIZE, "%.1f Kg",
                         info->weight);
  Unicode::snprintf(heightAreaBuffer, HEIGHTAREA_SIZE, "%d cm", info->height);

  invalidate();
}
