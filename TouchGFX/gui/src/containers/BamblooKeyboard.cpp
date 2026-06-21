#include "BitmapDatabase.hpp"
#include "gui/containers/BamblooKeyboardBase.hpp"
#include "images/BitmapDatabase.hpp"
#include "touchgfx/Color.hpp"
#include "touchgfx/TypedText.hpp"
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/containers/BamblooKeyboard.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/widgets/Keyboard.hpp>

#include <cstring>

static const bambloo::BamblooKeyboardBase::Key keyArray[30] = {
    {1, Rect(4 + 72 * 0, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {2, Rect(4 + 72 * 1, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {3, Rect(4 + 72 * 2, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {4, Rect(4 + 72 * 3, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {5, Rect(4 + 72 * 4, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {6, Rect(4 + 72 * 5, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {7, Rect(4 + 72 * 6, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {8, Rect(4 + 72 * 7, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {9, Rect(4 + 72 * 8, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {10, Rect(4 + 72 * 9, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {11, Rect(4 + 72 * 10, 64, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},

    {12, Rect(4 + 72 * 0, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {13, Rect(4 + 72 * 1, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {14, Rect(4 + 72 * 2, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {15, Rect(4 + 72 * 3, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {16, Rect(4 + 72 * 4, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {17, Rect(4 + 72 * 5, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {18, Rect(4 + 72 * 6, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {19, Rect(4 + 72 * 7, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {20, Rect(4 + 72 * 8, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {21, Rect(4 + 72 * 9, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {22, Rect(4 + 72 * 10, 127, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},

    {23, Rect(4 + 72 * 2, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {24, Rect(4 + 72 * 3, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {25, Rect(4 + 72 * 4, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {26, Rect(4 + 72 * 5, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {27, Rect(4 + 72 * 6, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {28, Rect(4 + 72 * 7, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {29, Rect(4 + 72 * 8, 190, 72, 63), BITMAP_KEY_PRESSED_LETTER_ID},
    {30, Rect(4 + 72 * 2, 253, 504, 63), BITMAP_KEY_PRESSED_SPACE_ID},
};

/**
 * Callback areas for the special buttons on the CustomKeyboard.
 */
static bambloo::BamblooKeyboardBase::CallbackArea callbackAreas[4] = {
    {Rect(4, 190, 144, 63), 0, BITMAP_KEY_PRESSED_SHIFT_ID},       // caps-lock
    {Rect(652, 190, 144, 63), 0, BITMAP_KEY_PRESSED_BACKSPACE_ID}, // backspace
    {Rect(4, 253, 144, 63), 0, BITMAP_KEY_PRESSED_MODE_ID},        // mode
    {Rect(652, 253, 144, 63), 0, BITMAP_KEY_PRESSED_RETURN_ID},    // backspace
};

static const bambloo::BamblooKeyboardBase::Layout layout = {
    BITMAP_KEYBOARD_ID,   // bitmap
    keyArray,             // keyArray
    30,                   // numberOfKeys
    callbackAreas,        // callbackAreaArray
    4,                    // numberOfCallbackAreas
    Rect(12, 6, 792, 53), // textAreaPosition
    TypedText(T_ENTEREDTEXT),
    TypedText(T_PASSWORDTEXT),
    touchgfx::Color::getColorFromRGB(0x55, 0x55, 0x55), // textAreaFontColor
    Typography::KEYBOARD,
    touchgfx::Color::getColorFromRGB(0x55, 0x55, 0x55),
};

static const Unicode::UnicodeChar keyMappingsAlphaLower[31] = {
    0,

    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '-',

    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '.', ',',

    'z', 'x', 'c', 'v', 'b', 'n', 'm',

    ' ',
};

static const Unicode::UnicodeChar keyMappingsAlphaUpper[31] = {
    0,

    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '-',

    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '.', ',',

    'Z', 'X', 'C', 'V', 'B', 'N', 'M',

    ' ',
};

static const Unicode::UnicodeChar keyMappingsNumLower[31] = {
    0,

    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',

    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '+',

    '=', '_', '<', '>', '/', '?', ';',

    ' ',
};

static const Unicode::UnicodeChar keyMappingsNumUpper[31] = {
    0,

    '~',    '`',    177,    215,    186,    96,     180,    '{', '}', '[', ']',

    0x00A5, 0x00A3, 0x20AC, 0x00A9, 0x00AB, 0x00BB, 0x00A2, '*', '(', ')', '+',

    0x2022, '_',    '<',    '>',    '/',    '?',    ';',

    ' ',
};

BamblooKeyboard::BamblooKeyboard()
    : capslockPressed(this, &BamblooKeyboard::capsLockPressedHandler),
      backspacePressed(this, &BamblooKeyboard::backspacePressedHandler),
      returnPressed(this, &BamblooKeyboard::returnPressedHandler),
      modePressed(this, &BamblooKeyboard::modePressedHandler), alphaMode(true),
      shiftMode(false) {

  ::layout.callbackAreaArray[0].callback = &capslockPressed;
  ::layout.callbackAreaArray[1].callback = &backspacePressed;
  ::layout.callbackAreaArray[2].callback = &modePressed;
  ::layout.callbackAreaArray[3].callback = &returnPressed;

  setLayout(&::layout);
  setPosition(0, 0, 800, 320);
  setTextIndentation();
  memset(buffer, 0, sizeof(buffer));
  setBuffer(buffer, 16);

  TypedText txt = (T_MODENUMTEXT);
  modeBtnTextArea.setTypedText(txt);
  uint16_t voff = (::layout.callbackAreaArray[2].keyArea.height -
                   txt.getFont()->getBaseline()) /
                  2;
  modeBtnTextArea.setPosition(::layout.callbackAreaArray[2].keyArea.x,
                              ::layout.callbackAreaArray[2].keyArea.y + voff,
                              ::layout.callbackAreaArray[2].keyArea.width,
                              txt.getFont()->getBaseline());

  modeBtnTextArea.setVisible(true);
  modeBtnTextArea.setColor(Color::getColorFromRGB(0xFF, 0xFF, 0xFF));

  setKeymappingList();
  setTouchable(true);
  add(modeBtnTextArea);
}

void BamblooKeyboard::capsLockPressedHandler() {
  shiftMode = !shiftMode;
  setKeymappingList();
}
void BamblooKeyboard::backspacePressedHandler() {
  if (bufferPosition) {
    setBufferPosition(bufferPosition - 1);
  }
}
void BamblooKeyboard::returnPressedHandler() {
  setVisible(false);
  if (bindedTextArea) {
    bindedTextArea->invalidate();
  }
  invalidate();
}
void BamblooKeyboard::modePressedHandler() {
  alphaMode = !alphaMode;
  setKeymappingList();
}

void BamblooKeyboard::setKeymappingList() {
  if (alphaMode) {
    if (shiftMode) {
      BamblooKeyboardBase::setKeymappingList(keyMappingsAlphaUpper);
    } else {
      BamblooKeyboardBase::setKeymappingList(keyMappingsAlphaLower);
    }
  } else {
    if (shiftMode) {
      BamblooKeyboardBase::setKeymappingList(keyMappingsNumUpper);
    } else {
      BamblooKeyboardBase::setKeymappingList(keyMappingsNumLower);
    }
  }
}

void BamblooKeyboard::initialize() {}

void BamblooKeyboard::setMode(int mode) {
  enteredText.setTypedText((mode & 1) ? layout->pswdAreaFont
                                      : layout->textAreaFont);
  if (mode & 2) {
    alphaMode = false;
  } else {
    alphaMode = true;
  }
  shiftMode = false;
  setKeymappingList();
}
