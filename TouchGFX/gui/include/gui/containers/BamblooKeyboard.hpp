#ifndef BAMBLOOKEYBOARD_HPP
#define BAMBLOOKEYBOARD_HPP

#include "gui/containers/BamblooKeyboardBase.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/containers/Container.hpp"
#include <gui/containers/BamblooKeyboardBase.hpp>
#include <touchgfx/widgets/Keyboard.hpp>

class BamblooKeyboard : public bambloo::BamblooKeyboardBase {
public:
  BamblooKeyboard();
  virtual ~BamblooKeyboard() {}

  virtual void initialize();

protected:
  Unicode::UnicodeChar getCharForKey(uint8_t keyId) const;
private:
  void backspacePressedHandler();
  void returnPressedHandler();
  void modePressedHandler();
  void capsLockPressedHandler();
  void keyPressedHandler(Unicode::UnicodeChar key);
  void setKeymappingList();

  TextArea modeBtnTextArea;
  Callback<BamblooKeyboard> capslockPressed;
  Callback<BamblooKeyboard> backspacePressed;
  Callback<BamblooKeyboard> returnPressed;
  Callback<BamblooKeyboard> modePressed;

  Callback<BamblooKeyboard, Unicode::UnicodeChar> keyPressed;

  bool alphaMode;
  bool shiftMode;

  Unicode::UnicodeChar buffer[32];
};

#endif // BAMBLOOKEYBOARD_HPP
