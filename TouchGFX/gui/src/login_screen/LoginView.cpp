#include "gui/common/FrontendApplication.hpp"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/Callback.hpp"
#include "touchgfx/TypedText.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
#include <gui/login_screen/LoginView.hpp>

#include "parameters.h"
#include "sqlite3.h"

LoginView::LoginView()
    : pinNumClickedCallback(this, &LoginView::pinNumClicked),
      funBtnClickedCallback(this, &LoginView::funBtnClicked),
      pinBtnClickedCallback(this, &LoginView::pinBtnClicked),
      pinTxtClickedCallback(this, &LoginView::pinTxtClicked),
      keyboardCallback(this, &LoginView::keyboardEventHandler), pinLen(0),
      pinVal(0) {}

void LoginView::setupScreen() {
  LoginViewBase::setupScreen();

  FrontendApplication::getInstance()->initDatabase();
  // FrontendApplication::getInstance()->testDatabase();
  
  btnPin0.setClickAction(pinNumClickedCallback);
  btnPin1.setClickAction(pinNumClickedCallback);
  btnPin2.setClickAction(pinNumClickedCallback);
  btnPin3.setClickAction(pinNumClickedCallback);
  btnPin4.setClickAction(pinNumClickedCallback);
  btnPin5.setClickAction(pinNumClickedCallback);
  btnPin6.setClickAction(pinNumClickedCallback);
  btnPin7.setClickAction(pinNumClickedCallback);
  btnPin8.setClickAction(pinNumClickedCallback);
  btnPin9.setClickAction(pinNumClickedCallback);

  btnBack.setClickAction(funBtnClickedCallback);
  btnCpin.setClickAction(funBtnClickedCallback);

  oldPinArea.setClickAction(pinTxtClickedCallback);
  newPinArea.setClickAction(pinTxtClickedCallback);
  reePinArea.setClickAction(pinTxtClickedCallback);

  btnCpinOkay.setClickAction(pinBtnClickedCallback);
  btnCpinExit.setClickAction(pinBtnClickedCallback);
}

void LoginView::tearDownScreen() { LoginViewBase::tearDownScreen(); }

void LoginView::pinNumClicked(const touchgfx::ButtonWithLabel &source,
                              const touchgfx::ClickEvent &evt) {
  if (evt.getType() != evt.PRESSED) {
    return;
  }

  if (pinLen >= PINAREA_SIZE - 1) {
    return;
  }

  pinVal = pinVal * 10 + (source.getLabelText().getText()[0] - '0');
  pinAreaBuffer[pinLen++] = 0x00B7;
  pinAreaBuffer[pinLen] = 0;

  if (pinLen == PINAREA_SIZE - 1) {
    if (parameters_check_pin(pinVal)) {
      application().gotoMainScreenNoTransition();
    }
    pinLen = 0;
    pinVal = 0;
    pinAreaBuffer[0] = 0;
  }
  pinArea.invalidate();
  // pinArea.resizeToCurrentTextWithAlignment();
}

void LoginView::funBtnClicked(const touchgfx::ButtonWithIcon &source,
                              const touchgfx::ClickEvent &evt) {
  if (evt.getType() != evt.RELEASED) {
    return;
  }
  if (pinLen) {
    pinAreaBuffer[--pinLen] = 0;
    pinVal /= 10;
    pinArea.invalidate();
    // pinArea.resizeToCurrentTextWithAlignment();
  }
}

void LoginView::pinTxtClicked(const touchgfx::TextAreaWithOneWildcard &source,
                              const touchgfx::ClickEvent &evt) {
  if (evt.getType() != evt.PRESSED) {
    return;
  }

  currentPinArea = &source;

  auto application = FrontendApplication::getInstance();
  application->attachKeyboardToCurrentScreen();
  application->setKeyboardBuffer(
      const_cast<Unicode::UnicodeChar *>(source.getWildcard()),
      OLDPINAREA_SIZE);
  application->setKeyboardCallback(keyboardCallback);
  application->showKeyboard(true);
}

void LoginView::keyboardEventHandler(Unicode::UnicodeChar c) {
  if (currentPinArea && !c) {
    currentPinArea->invalidate();
  }
}

static int32_t checkPwdPin(Unicode::UnicodeChar *code) {
  int32_t pin = 0;
  for (int i = 0; i < 6; i++) {
    Unicode::UnicodeChar c = *code++;
    if (!c) {
      return -1;
    }
    if (c >= '0' && c <= '9') {
      pin = pin * 10 + (c - '0');
    } else {
      return -1;
    }
  }
  return pin;
}

void LoginView::pinBtnClicked(const touchgfx::ButtonWithLabel &source,
                              const touchgfx::ClickEvent &evt) {
  if (evt.getType() != evt.RELEASED) {
    return;
  }
  if (&source == &btnCpinExit) {
    oldPinAreaBuffer[0] = 0;
    newPinAreaBuffer[0] = 0;
    reePinAreaBuffer[0] = 0;

    ChangePwdModal.hide();
    ChangePwdModal.invalidate();
  } else {
    uint32_t opin;
    uint32_t npin;
    if (((opin = checkPwdPin(oldPinAreaBuffer)) < 0) ||
        ((npin = checkPwdPin(newPinAreaBuffer)) < 0) ||
        ((npin = checkPwdPin(reePinAreaBuffer)) < 0)) {
      ErrorPrompt.setTypedText(TypedText(T_PINFORMATERROR));
      ErrorPrompt.invalidate();
      return;
    }
    if (Unicode::strncmp(newPinAreaBuffer, reePinAreaBuffer, 6)) {
      ErrorPrompt.setTypedText(TypedText(T_PINMISMATCH));
      ErrorPrompt.invalidate();
      return;
    }
    if (!parameters_check_pin(opin)) {
      ErrorPrompt.setTypedText(TypedText(T_PINMISMATCH));
      ErrorPrompt.invalidate();
      return;
    }
    parameters_save_pin(npin);
  }
}