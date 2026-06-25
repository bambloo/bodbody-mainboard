#include "touchgfx/hal/HAL.hpp"
#include <gui/common/FrontendApplication.hpp>

FrontendApplication::FrontendApplication(Model &m, FrontendHeap &heap)
    : FrontendApplicationBase(m, heap) {
  globalKeyboard.setVisible(false);
}

bool FrontendApplication::isKeyboardVisible() const {
  return globalKeyboard.isVisible();
}

void FrontendApplication::hideKeyboard() { globalKeyboard.setVisible(false); }

void FrontendApplication::showKeyboard(int mode) {

  globalKeyboard.setVisible(true);
  globalKeyboard.setMode(mode);
  globalKeyboard.invalidate();
}

void FrontendApplication::setKeyboardBuffer(Unicode::UnicodeChar *buffer,
                                            uint16_t size) {
  globalKeyboard.setBuffer(buffer, size);
}

void FrontendApplication::attachKeyboardToCurrentScreen() {
  auto *container = &getCurrentScreen()->getRootContainer();
  // Remove from previous parent if exists
  if (globalKeyboard.getParent()) {
    static_cast<Container *>(globalKeyboard.getParent())
        ->remove(globalKeyboard);
  }

  // Add to current screen
  container->add(globalKeyboard);

  // Position the keyboard (e.g., at the bottom)
  // You might want to store desired X/Y in members
  globalKeyboard.setXY(0, 280); // Example position
}

// void FrontendApplication::drawCachedAreas() {
//   if (!redraw.isEmpty()) {
//     invalidateArea(redraw);
//     redraw = {0, 0, 0, 0};
//   }
//   auto refresh_strategy = HAL::getInstance()->getFrameRefreshStrategy();

//   if (!cachedDirtyAreas.size() &&
//       refresh_strategy != HAL::REFRESH_STRATEGY_PARTIAL_BUFFER_TFT_CTRL) {
//     return;
//   }

//   RectVector_t backupDirtyAreas;
//   for (int i = 0; i < cachedDirtyAreas.size(); i++) {
//     backupDirtyAreas.add(cachedDirtyAreas[i]);
//   }

//   for (int i = 0; i < lastRects.size(); i++) {
//     invalidateArea(lastRects[i]);
//   }

//   if (refresh_strategy != touchgfx::HAL::REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER) {
//     for (int i = 0; i < cachedDirtyAreas.size(); i++) {
//       auto dirtyArea = cachedDirtyAreas[i];
//       if (!dirtyArea.isEmpty()) {
//         draw(dirtyArea);
//       }
//     }
//   }

//   if (HAL::USE_DOUBLE_BUFFERING) {
//     lastRects.clear();
//     for (int i = 0; i < backupDirtyAreas.size(); i++) {
//       lastRects.add(backupDirtyAreas[i]);
//     }
//   }
// }