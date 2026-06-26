#ifndef USERINFOLISTITEM_HPP
#define USERINFOLISTITEM_HPP

#include "bodbody-helper.h"
#include "touchgfx/Color.hpp"
#include "touchgfx/hal/Types.hpp"
#include <gui_generated/containers/UserInfoListItemBase.hpp>

class UserInfoListItem : public UserInfoListItemBase {
public:
  UserInfoListItem();
  virtual ~UserInfoListItem() {}

  virtual void initialize();

  void setUserInfo(bodbody_user_info_t *info);
  void setSelected(bool sel) {
    selected = sel;
    if (selected) {
      background.setColor(Color::getColorFromRGB(0xCC, 0xCC, 0xCC));
    } else {
      background.setColor(Color::getColorFromRGB(0xFF, 0xFA, 0xFA));
    }
    invalidate();
  };

protected:
private:
  bool selected;
};

#endif // USERINFOLISTITEM_HPP
