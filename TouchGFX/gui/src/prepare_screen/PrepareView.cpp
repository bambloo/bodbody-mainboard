#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include <gui/prepare_screen/PrepareView.hpp>

PrepareView::PrepareView()
    : btnClickedCallback(this, &PrepareView::btnClicked) {
  buttonQuery.setClickAction(btnClickedCallback);
  buttonAdd.setClickAction(btnClickedCallback);
  buttonMod.setClickAction(btnClickedCallback);
  buttonDel.setClickAction(btnClickedCallback);
}

void PrepareView::setupScreen() { PrepareViewBase::setupScreen(); }

void PrepareView::tearDownScreen() { PrepareViewBase::tearDownScreen(); }

void PrepareView::btnClicked(ButtonWithIcon *button, ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (button == &buttonAdd) {
    addModUserModal.setVisible(true);
    return;        
  }
}
