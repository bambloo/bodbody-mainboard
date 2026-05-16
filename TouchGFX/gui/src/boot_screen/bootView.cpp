#include <gui/boot_screen/bootView.hpp>

BootView::BootView() : clickCallback(this, &BootView::clickHandler) {}

void BootView::setupScreen() {
  BootViewBase::setupScreen();
  icon.setClickAction(clickCallback);
}

void BootView::tearDownScreen() { BootViewBase::tearDownScreen(); }

void BootView::clickHandler(const Image &img, const touchgfx::ClickEvent &evt) {
    if (evt.getType() == touchgfx::ClickEvent::RELEASED) {
        application().gotoMainScreenNoTransition();
    }
}
