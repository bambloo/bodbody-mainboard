#include "touchgfx/Callback.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
#include <gui/login_screen/LoginView.hpp>

LoginView::LoginView() :
    pinClickedCallback(this, &LoginView::pinClicked),
    funClickedCallback(this, &LoginView::funClicked),
    pinLen(0),
    pinVal(0)
{

}

void LoginView::setupScreen()
{
    LoginViewBase::setupScreen();
    btnPin0.setClickAction(pinClickedCallback);
    btnPin1.setClickAction(pinClickedCallback);
    btnPin2.setClickAction(pinClickedCallback);
    btnPin3.setClickAction(pinClickedCallback);
    btnPin4.setClickAction(pinClickedCallback);
    btnPin5.setClickAction(pinClickedCallback);
    btnPin6.setClickAction(pinClickedCallback);
    btnPin7.setClickAction(pinClickedCallback);
    btnPin8.setClickAction(pinClickedCallback);
    btnPin9.setClickAction(pinClickedCallback);
    btnBack.setClickAction(funClickedCallback);
}

void LoginView::tearDownScreen()
{
    LoginViewBase::tearDownScreen();
}

void LoginView::pinClicked(const touchgfx::ButtonWithLabel& source, const touchgfx::ClickEvent& evt)
{
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
        presenter->checkPin(pinVal);
        pinLen = 0;
        pinVal = 0;
    }
    pinArea.invalidate();
    // pinArea.resizeToCurrentTextWithAlignment();
}

void LoginView::funClicked(const touchgfx::ButtonWithIcon& source, const touchgfx::ClickEvent& evt)
{
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