#include "BitmapDatabase.hpp"
#include "images/BitmapDatabase.hpp"
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/containers/BamblooKeyboard.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/widgets/Keyboard.hpp>

#include <cstring>

static const Keyboard::Key keyArray[30] = {
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
static Keyboard::CallbackArea callbackAreas[4] = {
    {Rect(4, 190, 144, 63), 0, BITMAP_KEY_PRESSED_SHIFT_ID},       // caps-lock
    {Rect(652, 190, 144, 63), 0, BITMAP_KEY_PRESSED_BACKSPACE_ID}, // backspace
    {Rect(4, 253, 144, 63), 0, BITMAP_KEY_PRESSED_MODE_ID},        // mode
    {Rect(652, 253, 144, 63), 0, BITMAP_KEY_PRESSED_RETURN_ID},    // backspace
};

static const Keyboard::Layout layout = {
    BITMAP_KEYBOARD_ID,  // bitmap
    keyArray,            // keyArray
    30,                  // numberOfKeys
    callbackAreas,       // callbackAreaArray
    4,                   // numberOfCallbackAreas
    Rect(4, 6, 792, 53), // textAreaPosition
    TypedText(T_ENTEREDTEXT),
    0xFFFFFFFF, // textAreaFontColor
    Typography::KEYBOARD,
};

static const Keyboard::KeyMapping keyMappingsAlphaLower[30] = {
    {1, 113},  // q
    {2, 119},  // w
    {3, 101},  // e
    {4, 114},  // r
    {5, 116},  // t
    {6, 121},  // y
    {7, 117},  // u
    {8, 105},  // i
    {9, 111},  // o
    {10, 112}, // p
    {11, 45},  // -
    {12, 97},  // a
    {13, 115}, // s
    {14, 100}, // d
    {15, 102}, // f
    {16, 103}, // g
    {17, 104}, // h
    {18, 106}, // j
    {19, 107}, // k
    {20, 108}, // l
    {21, 46},  // .
    {22, 44},  // ,
    {23, 122}, // z
    {24, 120}, // x
    {25, 99},  // c
    {26, 118}, // v
    {27, 98},  // b
    {28, 110}, // n
    {29, 109}, // m
    {30, 32},  // space
};

static const Keyboard::KeyMappingList keyMappingListAlphaLower = {
    keyMappingsAlphaLower, 1};

static const Keyboard::KeyMapping keyMappingsAlphaUpper[30] = {
    {1, 113 - 32},  // Q
    {2, 119 - 32},  // W
    {3, 101 - 32},  // E
    {4, 114 - 32},  // R
    {5, 116 - 32},  // T
    {6, 121 - 32},  // Y
    {7, 117 - 32},  // U
    {8, 105 - 32},  // I
    {9, 111 - 32},  // O
    {10, 112 - 32}, // P
    {11, 47},       // /
    {12, 97 - 32},  // A
    {13, 115 - 32}, // S
    {14, 100 - 32}, // D
    {15, 102 - 32}, // F
    {16, 103 - 32}, // G
    {17, 104 - 32}, // H
    {18, 106 - 32}, // J
    {19, 107 - 32}, // K
    {20, 108 - 32}, // L
    {21, 58},       // :
    {22, 59},       //
    {23, 122 - 32}, // Z
    {24, 120 - 32}, // X
    {25, 99 - 32},  // C
    {26, 118 - 32}, // V
    {27, 98 - 32},  // B
    {28, 110 - 32}, // N
    {29, 109 - 32}, // M
    {30, 32}        // space
};

static const Keyboard::KeyMappingList keyMappingListAlphaUpper = {
    keyMappingsAlphaUpper, 1};

static const Keyboard::KeyMapping keyMappingsNumLower[30] = {
    {1, 49},  // 1
    {2, 50},  // 2
    {3, 51},  // 3
    {4, 52},  // 4
    {5, 53},  // 5
    {6, 54},  // 6
    {7, 55},  // 7
    {8, 56},  // 8
    {9, 57},  // 9
    {10, 48}, // 0
    {11, 64}, // @

    {12, 35}, // #
    {13, 36}, // $
    {14, 37}, // %
    {15, 38}, // &
    {16, 42}, // *
    {17, 45}, // -
    {18, 43}, // +
    {19, 40}, // (
    {20, 41}, // )
    {21, 33}, // !
    {22, 34}, // "

    {23, 39}, // '
    {24, 58}, // :
    {25, 59}, // ;
    {26, 47}, // /
    {27, 63}, // ?
    {28, 44}, // ,
    {29, 46}, // .

    {30, 32} // space
};

static const Keyboard::KeyMappingList keyMappingListNumLower = {
    keyMappingsNumLower, 1};

static const Keyboard::KeyMapping keyMappingsNumUpper[30] = {
    {1, 126},  // ~
    {2, 177},  // +/-
    {3, 215},  // x
    {4, 247},  // -
    {5, 8226}, // *
    {6, 186},  // o
    {7, 96},   // '
    {8, 180},  // '
    {9, 123},  // {
    {10, 125}, // }
    {11, 169}, // (c)

    {12, 163},  // pound
    {13, 8364}, // euro
    {14, 94},   // ^
    {15, 174},  // (R)
    {16, 165},  // yen
    {17, 95},   // _
    {18, 61},   // =
    {19, 91},   // [
    {20, 93},   // ]
    {21, 60},   // <
    {22, 62},   // >

    {23, 161}, // !
    {24, 162}, // cent
    {25, 124}, // |
    {26, 92},  // \ .
    {27, 191}, // ?
    {28, 44},  // ,
    {29, 46},  // .

    {30, 32} // space
};

static const Keyboard::KeyMappingList keyMappingListNumUpper = {
    keyMappingsNumUpper, 1};

BamblooKeyboard::BamblooKeyboard()
    : modeBtnTextArea(),
      capslockPressed(this, &BamblooKeyboard::capsLockPressedHandler),
      backspacePressed(this, &BamblooKeyboard::backspacePressedHandler),
      returnPressed(this, &BamblooKeyboard::returnPressedHandler),
      modePressed(this, &BamblooKeyboard::modePressedHandler),
      keyPressed(this, &BamblooKeyboard::keyPressedHandler), alphaMode(true),
      shiftMode(false) {
  ::layout.callbackAreaArray[0].callback = &capslockPressed;
  ::layout.callbackAreaArray[1].callback = &backspacePressed;
  ::layout.callbackAreaArray[2].callback = &modePressed;
  ::layout.callbackAreaArray[3].callback = &returnPressed;

  setLayout(&::layout);
  setKeyListener(keyPressed);
  setPosition(0, 0, 800, 320);
  setTextIndentation();
  memset(buffer, 0, sizeof(buffer));
  setBuffer(buffer, 32);

  setBufferPosition(0);
  setKeymappingList();
  setTouchable(true);
}

void BamblooKeyboard::capsLockPressedHandler() {
    shiftMode = !shiftMode;
    setKeymappingList();
}
void BamblooKeyboard::backspacePressedHandler() {}
void BamblooKeyboard::returnPressedHandler() {
  setVisible(false);
  invalidate();
}
void BamblooKeyboard::modePressedHandler() {
    alphaMode = !alphaMode;
    setKeymappingList();
}
void BamblooKeyboard::keyPressedHandler(Unicode::UnicodeChar key) {}

void BamblooKeyboard::setKeymappingList() {
    if (alphaMode) {
        if (shiftMode) {
            Keyboard::setKeymappingList(&keyMappingListAlphaUpper);
        } else {
            Keyboard::setKeymappingList(&keyMappingListAlphaLower);
        }
    } else {
        if (shiftMode) {
            Keyboard::setKeymappingList(&keyMappingListNumUpper);
        } else {
            Keyboard::setKeymappingList(&keyMappingListNumLower);
        }   
    }
}

void BamblooKeyboard::initialize() {}
