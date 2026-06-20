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
  void setMode(int mode);

protected:
  Unicode::UnicodeChar getCharForKey(uint8_t keyId) const;

private:
  void backspacePressedHandler();
  void returnPressedHandler();
  void modePressedHandler();
  void capsLockPressedHandler();
  void setKeymappingList();

  TextArea modeBtnTextArea;
  Callback<BamblooKeyboard> capslockPressed;
  Callback<BamblooKeyboard> backspacePressed;
  Callback<BamblooKeyboard> returnPressed;
  Callback<BamblooKeyboard> modePressed;

  Callback<BamblooKeyboard, Unicode::UnicodeChar> keyPressed;

  Unicode::UnicodeChar buffer[32];

  bool alphaMode;
  bool shiftMode;
};

#endif // BAMBLOOKEYBOARD_HPP
