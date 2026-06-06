#include "touchgfx/Callback.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
#include <gui/login_screen/LoginView.hpp>

LoginView::LoginView() :
    pinClickedCallback(this, &LoginView::pinClicked),
    pinLen(0)
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
    btnPin9.setClickAction(pinClickedCallback);}

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
    pinAreaBuffer[pinLen++] = source.getLabelText().getText()[0];
    pinAreaBuffer[pinLen] = 0;
    centerPinArea();
    pinArea.invalidate();
}

void LoginView::centerPinArea()
{
    pinArea.resizeToCurrentText();
    // Get the parent container's dimensions (usually the screen or a group)
    int16_t parentWidth = getScreenWidth();
    // int16_t parentHeight = getScreenHeight();

    // // Get the current dimensions of pinArea
    int16_t areaWidth = pinArea.getWidth();
    int16_t areaHeight = pinArea.getHeight();

    // // Calculate centered position
    int16_t x = (parentWidth - areaWidth) / 2;
    int16_t y = pinArea.getY(); // Keep the current Y position, or calculate if needed
    // int16_t y = (parentHeight - areaHeight) / 2;

    // Set the new position
    pinArea.resizeToCurrentTextWithAlignment();
    pinArea.setPosition(x, y, areaWidth, areaHeight);
}