#include "gui/common/FrontendApplication.hpp"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/TypedText.hpp"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include <gui/prepare_screen/PrepareView.hpp>

PrepareView::PrepareView()
    : btnClickedCallback(this, &PrepareView::btnClicked) {
  buttonQuery.setClickAction(btnClickedCallback);
  buttonAdd.setClickAction(btnClickedCallback);
  buttonMod.setClickAction(btnClickedCallback);
  buttonDel.setClickAction(btnClickedCallback);
  buttonDone.setClickAction(btnClickedCallback);
  buttonCancel.setClickAction(btnClickedCallback);
}

void PrepareView::setupScreen() { PrepareViewBase::setupScreen(); }

void PrepareView::tearDownScreen() { PrepareViewBase::tearDownScreen(); }

void PrepareView::btnClicked(const ButtonWithIcon &button,
                             const ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (&button == &buttonAdd) {
    profileMode = 1;
    currMenuLabel.setTypedText(TypedText(T_ADDUSERLABEL));
    addModUserModal.setVisible(true);
    addModUserModal.invalidate();
    return;
  }

  if (&button == &buttonCancel) {
    profileMode = 0;
    addModUserModal.setVisible(false);
    currMenuLabel.setTypedText(TypedText(T_CHOOSEUSERLABEL));
    invalidate();
    return;
  }
}
