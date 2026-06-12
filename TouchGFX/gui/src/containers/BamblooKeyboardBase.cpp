#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/Bitmap.hpp"
#include "touchgfx/Color.hpp"
#include "touchgfx/Drawable.hpp"
#include "touchgfx/FontManager.hpp"
#include "touchgfx/containers/Container.hpp"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/hal/Types.hpp"
#include "touchgfx/lcd/LCD.hpp"
#include <gui/containers/BamblooKeyboardBase.hpp>

#include <cmath>

using namespace touchgfx;
using namespace bambloo;
BamblooKeyboardBase::BamblooKeyboardBase()
    : Container(), keyListener(nullptr), buffer(nullptr), bufferSize(0),
      bufferPosition(0), layout(nullptr), keyMappingList(nullptr),
      cancelIsEmitted(false) {
  image.setXY(0, 0);
  highlightImage.setVisible(false);

  add(image);
  add(highlightImage);
  add(enteredText);
}

void BamblooKeyboardBase::setBuffer(Unicode::UnicodeChar *newBuffer,
                                    uint16_t newBufferSize) {
  buffer = newBuffer;
  bufferSize = newBufferSize;

  enteredText.setWildcard(buffer);
  if (buffer) {
    bufferPosition = Unicode::strlen(buffer);
  } else {
    bufferPosition = 0;
  }
}

void BamblooKeyboardBase::setLayout(const Layout *newLayout) {

  // 1. Store the layout reference
  layout = newLayout;

  // 2. Safely apply the layout configuration if the pointer is valid
  if (layout != nullptr) {
    // Set the background bitmap graphics
    image.setBitmap(Bitmap(layout->bitmap));

    // Configure typography, color, and location for the text display
    enteredText.setTypedText(layout->textAreaFont);
    enteredText.setColor(layout->textAreaFontColor);
    enteredText.setPosition(
        layout->textAreaPosition.x, layout->textAreaPosition.y,
        layout->textAreaPosition.width, layout->textAreaPosition.height);
  }

  // 3. Invalidate the widget to force a fresh render on screen
  invalidate();
}

void BamblooKeyboardBase::setTextIndentation() {
  if (layout != nullptr) {
    // 2. Fetch the automatic text padding / indentation from the layout's font
    // settings
    uint8_t fontPadding = layout->textAreaFont.getFont()->getMaxPixelsLeft();

    uint16_t line_height = layout->textAreaFont.getFont()->getBaseline() + 6;
    uint16_t voff = (layout->textAreaPosition.height - line_height) / 2;

    // 3. Re-adjust the text container dimensions safely to prevent clipping
    enteredText.setPosition(layout->textAreaPosition.x - fontPadding,
                            layout->textAreaPosition.y + voff,
                            layout->textAreaPosition.width + (2 * fontPadding),
                            line_height);

    // 4. Force a repaint of the text area box
    enteredText.invalidate();
  }
}

void BamblooKeyboardBase::setPasswordMode(bool mode) {
  enteredText.setTypedText(mode ? layout->pswdAreaFont : layout->textAreaFont);
}

void BamblooKeyboardBase::setBufferPosition(uint16_t newPos) {
  bufferPosition = newPos;
  buffer[bufferPosition] = 0;
  enteredText.invalidate();
}

void BamblooKeyboardBase::setKeymappingList(
    const Unicode::UnicodeChar *newKeyMappingList) {
  keyMappingList = newKeyMappingList;
  invalidate();
}

void BamblooKeyboardBase::setupDrawChain(const Rect &invalidatedArea,
                                         Drawable **nextPreviousElement) {
  Container::setupDrawChain(invalidatedArea, nextPreviousElement);
  resetDrawChainCache();
  nextDrawChainElement = *nextPreviousElement;
  *nextPreviousElement = this;
}

void BamblooKeyboardBase::handleDragEvent(const DragEvent &event) {
  if (highlightImage.isVisible() &&
      !highlightImage.getRect().intersect(event.getNewX(), event.getNewY())) {
    if (!cancelIsEmitted) {
      ClickEvent ce(ClickEvent::CANCEL, event.getOldX(), event.getOldY());
      handleClickEvent(ce);
    }
  }
}

void BamblooKeyboardBase::draw(const Rect &invalidatedArea) const {
  if (!layout) {
    return;
  }

  auto font = FontManager::getFont(layout->keyFont);
  if (!font) {
    return;
  }

  LCD::StringVisuals stringVisuals;
  stringVisuals.font = font;
  stringVisuals.alignment = CENTER;
  stringVisuals.alpha = 255;

  stringVisuals.color.color = layout->keyFontColor.color;

  int fontHeight = font->getHeight();
  auto keyCursor = layout->keyArray;
  auto keyCursorEnd = layout->keyArray + layout->numberOfKeys;

  while (keyCursor < keyCursorEnd) {
    if (!keyCursor->keyArea.intersect(invalidatedArea)) {
      keyCursor++;
      continue;
    }

    Unicode::UnicodeChar code = keyMappingList[keyCursor->keyId];
    if (!code) {
      keyCursor++;
      continue;
    }

    int16_t voff = (keyCursor->keyArea.height - fontHeight) / 2;
    Rect rect2Draw = keyCursor->keyArea;
    rect2Draw.y += voff;
    rect2Draw.height = fontHeight;

    if (invalidatedArea.intersect(rect2Draw)) {
      Unicode::UnicodeChar str[] = {code, 0};

      Rect t(0, 0, rect2Draw.width, rect2Draw.height);
      translateRectToAbsolute(rect2Draw);
      touchgfx::HAL::lcd().drawString(rect2Draw, t, stringVisuals, str);
    }
    keyCursor++;
  }
}

void BamblooKeyboardBase::handleClickEvent(const ClickEvent &event) {
  auto eventType = event.getType();
  if (eventType == ClickEvent::RELEASED && cancelIsEmitted) {
    cancelIsEmitted = false;
    return;
  }
  int16_t clickX = event.getX();
  int16_t clickY = event.getY();

  CallbackArea callbackArea;
  if (getCallbackAreaForCoordinates(&callbackArea, clickX, clickY)) {
    if (eventType == ClickEvent::PRESSED) {
      highlightImage.setXY(callbackArea.keyArea.x, callbackArea.keyArea.y);
      Bitmap bmp = Bitmap(callbackArea.highlightBitmapId);
      highlightImage.setBitmap(bmp);
      highlightImage.setVisible(true);
      highlightImage.invalidate();
      return;
    } else if (callbackArea.callback && eventType == ClickEvent::RELEASED) {
      callbackArea.callback->execute();
    }
  } else {
    Key k;
    if (getKeyForCoordinates(&k, clickX, clickY)) {
      if (eventType == ClickEvent::PRESSED) {
        highlightImage.setXY(k.keyArea.x, k.keyArea.y);
        Bitmap bmp = Bitmap(k.highlightBitmapId);
        highlightImage.setBitmap(bmp);
        highlightImage.setVisible(true);
        highlightImage.invalidate();
        return;
      } else {
        if (keyListener && eventType == ClickEvent::RELEASED) {
          Unicode::UnicodeChar code = keyMappingList[k.keyId];
          if (!code || bufferPosition >= bufferSize - 1) {
            goto end;
          }
          enteredText.invalidateContent();
          buffer[bufferPosition++] = code;
          buffer[bufferPosition] = 0;
          enteredText.invalidateContent();
          keyListener->execute(k.keyId);
        }
      }
    }
  }
end:
  highlightImage.setVisible(false);
  highlightImage.invalidate();
  if (eventType == ClickEvent::CANCEL) {
    cancelIsEmitted = true;
  }
}

bool BamblooKeyboardBase::getCallbackAreaForCoordinates(CallbackArea *area,
                                                        int16_t x,
                                                        int16_t y) const {

  if (!layout)
    return false;

  auto layoutAreaCursor = layout->callbackAreaArray;
  int count = layout->numberOfCallbackAreas;
  while (count--) {
    if (layoutAreaCursor->keyArea.intersect(x, y)) {
      *area = *layoutAreaCursor;
      return true;
    }
    layoutAreaCursor++;
  }
  return false;
}

bool BamblooKeyboardBase::getKeyForCoordinates(Key *key, int16_t x,
                                               int16_t y) const {
  if (!layout)
    return false;

  auto keyCursor = layout->keyArray;
  int count = layout->numberOfKeys;
  while (count--) {
    if (keyCursor->keyArea.intersect(x, y)) {
      *key = *keyCursor;
      return true;
    }
    keyCursor++;
  }
  return false;
}