#ifndef PREPAREVIEW_HPP
#define PREPAREVIEW_HPP

#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include <gui/prepare_screen/PreparePresenter.hpp>
#include <gui_generated/prepare_screen/PrepareViewBase.hpp>


class PrepareView : public PrepareViewBase {
public:
  PrepareView();
  virtual ~PrepareView() {}
  virtual void setupScreen();
  virtual void tearDownScreen();

protected:
  Callback<PrepareView, ButtonWithIcon &, ClickEvent &> btnClickedCallback;

private:
  void btnClicked(ButtonWithIcon &sender, ClickEvent &event);
};

#endif // PREPAREVIEW_HPP
