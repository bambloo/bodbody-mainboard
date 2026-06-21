#ifndef PREPAREVIEW_HPP
#define PREPAREVIEW_HPP

#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
#include "touchgfx/widgets/TextAreaWithWildcard.hpp"
#include <gui/prepare_screen/PreparePresenter.hpp>
#include <gui_generated/prepare_screen/PrepareViewBase.hpp>
#include <sys/types.h>
#include "bodbody-helper.h"

class PrepareView : public PrepareViewBase {
public:
  PrepareView();
  virtual ~PrepareView() {}
  virtual void setupScreen();
  virtual void tearDownScreen();

protected:
  Callback<PrepareView, const ButtonWithIcon &, const ClickEvent &>
      btnClickedCallback;
  Callback<PrepareView, const TextAreaWithOneWildcard &, const ClickEvent &>
      textClickedCallback;
  Callback<PrepareView, const ButtonWithLabel &, const ClickEvent &>
      genderClickedCallback;

private:
  uint8_t profileMode;
  bodbody_user_info_t profile;

  void updateProfile(uint8_t mode);
  void btnClicked(const ButtonWithIcon &sender, const ClickEvent &event);
  void textClicked(const TextAreaWithOneWildcard &sender,
                   const ClickEvent &event);
  void genderClicked(const ButtonWithLabel &sender, const ClickEvent &event);
};

#endif // PREPAREVIEW_HPP
