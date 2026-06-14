#include "sqlite3.h"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/Texts.hpp"
#include "tx_api.h"
#include <gui/boot_screen/bootView.hpp>

BootView::BootView() {}

sqlite3 *sqlite;

void boot_timeout(ULONG id) {
  BootView *view = (BootView *)id;
  view->gotoLoginView();
}
void BootView::setupScreen() {
  BootViewBase::setupScreen();
  Texts::setLanguage(ZH);

  int rc = sqlite3_initialize();
  tx_timer_create(&timer, (CHAR *)"boot timer", boot_timeout, (ULONG)this, 3000,
                  0, TX_AUTO_ACTIVATE);
}

void BootView::tearDownScreen() { BootViewBase::tearDownScreen(); }

void BootView::gotoLoginView() { application().gotoLoginScreenNoTransition(); }
