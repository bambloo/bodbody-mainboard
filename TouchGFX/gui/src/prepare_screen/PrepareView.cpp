#include "bodbody-helper.h"
#include "gui/common/FrontendApplication.hpp"
#include "gui_generated/common/FrontendApplicationBase.hpp"
#include "sqlite3.h"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/TypedText.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "utils.h"
#include <gui/prepare_screen/PrepareView.hpp>
#include <string.h>

PrepareView::PrepareView()
    : btnClickedCallback(this, &PrepareView::btnClicked),
      textClickedCallback(this, &PrepareView::textClicked),
      genderClickedCallback(this, &PrepareView::genderClicked) {
  buttonQuery.setClickAction(btnClickedCallback);
  buttonAdd.setClickAction(btnClickedCallback);
  buttonMod.setClickAction(btnClickedCallback);
  buttonDel.setClickAction(btnClickedCallback);
  buttonDone.setClickAction(btnClickedCallback);
  buttonCancel.setClickAction(btnClickedCallback);

  userArea.setClickAction(textClickedCallback);
  ageArea.setClickAction(textClickedCallback);
  heightArea.setClickAction(textClickedCallback);

  genderButton.setClickAction(genderClickedCallback);
}

void PrepareView::setupScreen() {
  PrepareViewBase::setupScreen();
  FrontendApplication::getInstance()->attachKeyboardToCurrentScreen();
  FrontendApplication::getInstance()->hideKeyboard();

  profile.age = 25;
  profile.gender = 'M';
  profile.height = 180;
  profile.weight = 70;
  profile.name[0] = 0;

  updateProfile(0);
}

void PrepareView::tearDownScreen() { PrepareViewBase::tearDownScreen(); }

// 0 - profile to ui
void PrepareView::updateProfile(uint8_t mode) {
  if (mode == 0) {
    Unicode::fromUTF8((const uint8_t *)profile.name, userAreaBuffer,
                      USERAREA_SIZE);
    Unicode::snprintf(ageAreaBuffer, AGEAREA_SIZE, "%d", profile.age);
    Unicode::snprintf(heightAreaBuffer, HEIGHTAREA_SIZE, "%d", profile.height);
  }

  if (mode == 1) {
    Unicode::toUTF8(userAreaBuffer, (uint8_t *)profile.name, USERAREA_SIZE);
    profile.age = Unicode::atoi(ageAreaBuffer);
    profile.height = Unicode::atoi(heightAreaBuffer);
    if (profileMode == 1) {
      profile.weight = 0;
    }
  }
}

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

  if (&button == &buttonDone) {
    updateProfile(1);
    uint8_t res =
        bodbody_save_user_info(FrontendApplication::getDatabase(), &profile);
    if (res == SQLITE_DONE) {
      addModUserModal.setVisible(false);
      invalidate();
    }
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

    FrontendApplication::getInstance()->bindKeyboardTextArea(userArea,
                                                             USERAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(0);
    return;
  }

  if (&textArea == &ageArea) {
    FrontendApplication::getInstance()->bindKeyboardTextArea(ageArea,
                                                             AGEAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(2);
    return;
  }

  if (&textArea == &heightArea) {
    FrontendApplication::getInstance()->bindKeyboardTextArea(heightArea,
                                                             HEIGHTAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(2);
    return;
  }
}

void PrepareView::genderClicked(const ButtonWithLabel &button,
                                const ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (profile.gender == 'M') {
    profile.gender = 'F';
    genderButton.setLabelText(TypedText(T_FEMALE));
    genderButton.invalidate();
  } else {
    profile.gender = 'M';
    genderButton.setLabelText(TypedText(T_MALE));
    genderButton.invalidate();
  }
}