#ifndef PREPAREVIEW_HPP
#define PREPAREVIEW_HPP

#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "touchgfx/widgets/TextAreaWithWildcard.hpp"
#include <gui/prepare_screen/PreparePresenter.hpp>
#include <gui_generated/prepare_screen/PrepareViewBase.hpp>
#include <sys/types.h>

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
private:
  uint8_t profileMode;
  void btnClicked(const ButtonWithIcon &sender, const ClickEvent &event);
  void textClicked(const TextAreaWithOneWildcard &sender, const ClickEvent &event);
};

#endif // PREPAREVIEW_HPP
