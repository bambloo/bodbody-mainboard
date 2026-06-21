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
