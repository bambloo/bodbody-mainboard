#include "gui/common/FrontendApplication.hpp"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/Callback.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
#include <gui/login_screen/LoginView.hpp>

#include "parameters.h"

LoginView::LoginView()
    : pinNumClickedCallback(this, &LoginView::pinNumClicked),
      funBtnClickedCallback(this, &LoginView::funBtnClicked),
      pinBtnClickedCallback(this, &LoginView::pinBtnClicked),
      pinTxtClickedCallback(this, &LoginView::pinTxtClicked),
      keyboardCallback(this, &LoginView::keyboardEventHandler), pinLen(0),
      pinVal(0) {}

void LoginView::setupScreen() {
  LoginViewBase::setupScreen();
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
  if (evt.getType() != evt.PRESSED) {
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
  application->showKeyboard();
}

void LoginView::keyboardEventHandler(Unicode::UnicodeChar c) {
  if (currentPinArea && !c) {
    currentPinArea->invalidate();
  }
}

void LoginView::pinBtnClicked(const touchgfx::ButtonWithLabel &source,
                              const touchgfx::ClickEvent &evt) {
  if (&source == &btnCpinExit) {
    oldPinAreaBuffer[0] = 0;
    newPinAreaBuffer[0] = 0;
    reePinAreaBuffer[0] = 0;

    ChangePwdModal.hide();
    ChangePwdModal.invalidate();
  }
}