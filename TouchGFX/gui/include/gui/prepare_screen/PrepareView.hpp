#ifndef PREPAREVIEW_HPP
#define PREPAREVIEW_HPP

#include "bodbody-helper.h"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
#include "touchgfx/widgets/TextAreaWithWildcard.hpp"
#include <gui/prepare_screen/PreparePresenter.hpp>
#include <gui_generated/prepare_screen/PrepareViewBase.hpp>
#include <sys/types.h>


#define PREPARE_VIEW_MAX_ITEM_PER_PAGE 7

class PrepareView : public PrepareViewBase {
public:
  PrepareView();
  virtual ~PrepareView() {}
  virtual void setupScreen();
  virtual void tearDownScreen();
  virtual void userInfoListUpdateItem(UserInfoListItem& item, int16_t itemIndex);

protected:
  Callback<PrepareView, const ButtonWithIcon &, const ClickEvent &>
      btnClickedCallback;
  Callback<PrepareView, const TextAreaWithOneWildcard &, const ClickEvent &>
      textClickedCallback;
  Callback<PrepareView, const ButtonWithLabel &, const ClickEvent &>
      genderClickedCallback;

private:
  uint8_t profileMode;
  uint8_t currentPage;
  uint8_t totalPage;

  bodbody_user_info_t userInfos[PREPARE_VIEW_MAX_ITEM_PER_PAGE];
  bodbody_user_info_t profile;

  void updateUserInfoList();
  void switchPage(uint8_t page);

  void updateProfile(uint8_t mode);
  void btnClicked(const ButtonWithIcon &sender, const ClickEvent &event);
  void textClicked(const TextAreaWithOneWildcard &sender,
                   const ClickEvent &event);
  void genderClicked(const ButtonWithLabel &sender, const ClickEvent &event);
};

#endif // PREPAREVIEW_HPP
