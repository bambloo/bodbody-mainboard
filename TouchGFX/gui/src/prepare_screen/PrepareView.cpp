#include "gui/common/FrontendApplication.hpp"
#include "gui_generated/common/FrontendApplicationBase.hpp"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/TypedText.hpp"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include <gui/prepare_screen/PrepareView.hpp>

PrepareView::PrepareView()
    : btnClickedCallback(this, &PrepareView::btnClicked),
      textClickedCallback(this, &PrepareView::textClicked) {
  buttonQuery.setClickAction(btnClickedCallback);
  buttonAdd.setClickAction(btnClickedCallback);
  buttonMod.setClickAction(btnClickedCallback);
  buttonDel.setClickAction(btnClickedCallback);
  buttonDone.setClickAction(btnClickedCallback);
  buttonCancel.setClickAction(btnClickedCallback);

  userArea.setClickAction(textClickedCallback);
  ageArea.setClickAction(textClickedCallback);
  heightArea.setClickAction(textClickedCallback);
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

void PrepareView::textClicked(const TextAreaWithOneWildcard &textArea,
                              const ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (&textArea == &userArea) {
    if (profileMode != 1) {
      return;
    }

    FrontendApplication::getInstance()->showKeyboard(0);
    FrontendApplication::getInstance()->setKeyboardBuffer(userAreaBuffer,
                                                          USERAREA_SIZE);
  }
}