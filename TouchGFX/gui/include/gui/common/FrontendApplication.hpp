#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include "gui/containers/BamblooKeyboard.hpp"
#include "touchgfx/Callback.hpp"
#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase {
public:
  FrontendApplication(Model &m, FrontendHeap &heap);
  virtual ~FrontendApplication() {}

  virtual void handleTickEvent() {
    model.tick();
    FrontendApplicationBase::handleTickEvent();
  }

  void showKeyboard(bool passwordMode = false);
  void hideKeyboard();
  bool isKeyboardVisible() const;
  void setKeyboardBuffer(Unicode::UnicodeChar *buffer, uint16_t size);
  void setKeyboardCallback(GenericCallback<Unicode::UnicodeChar> &callback);
  void attachKeyboardToCurrentScreen();

  static FrontendApplication *getInstance() {
    return static_cast<FrontendApplication *>(Application::getInstance());
  }

private:
  BamblooKeyboard globalKeyboard;
};

#endif // FRONTENDAPPLICATION_HPP
