#include "texts/TextKeysAndLanguages.hpp"
#include "tx_api.h"
#include <gui/boot_screen/bootView.hpp>
#include "touchgfx/Texts.hpp"



BootView::BootView() {}

void boot_timeout (ULONG id) {
    BootView* view = (BootView*)id;
    view->gotoLoginView();
}
void BootView::setupScreen() {
  BootViewBase::setupScreen();
  Texts::setLanguage(ZH);
  tx_timer_create(&timer, (CHAR*)"boot timer", boot_timeout, (ULONG)this, 3000, 0, TX_AUTO_ACTIVATE);
}

void BootView::tearDownScreen() { BootViewBase::tearDownScreen(); }

void BootView::gotoLoginView() {
    application().gotoLoginScreenNoTransition();
}
