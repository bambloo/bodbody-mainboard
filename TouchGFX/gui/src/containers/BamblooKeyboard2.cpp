#include "touchgfx/Bitmap.hpp"
#include "touchgfx/Color.hpp"
#include "touchgfx/Drawable.hpp"
#include "touchgfx/FontManager.hpp"
#include "touchgfx/containers/Container.hpp"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/hal/Types.hpp"
#include "touchgfx/lcd/LCD.hpp"
#include <gui/containers/BamblooKeyboard2.hpp>

using namespace touchgfx;
BamblooKeyboard2::BamblooKeyboard2()
    : Container(), keyListener(nullptr), buffer(nullptr), bufferSize(0),
      bufferPosition(0), layout(nullptr), keyMappingList(nullptr),
      cancelIsEmitted(false) {
  image.setXY(0, 0);

  add(image);
  highlightImage.setVisible(false);
  add(highlightImage);
  add(enteredText);
}

void BamblooKeyboard2::setBuffer(Unicode::UnicodeChar *newBuffer,
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

void BamblooKeyboard2::setLayout(const Layout *newLayout) {

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

void BamblooKeyboard2::setTextIndentation() {
  if (layout != nullptr) {
    // 2. Fetch the automatic text padding / indentation from the layout's font
    // settings
    uint8_t fontPadding = layout->textAreaFont.getFont()->getMaxPixelsLeft();

    // 3. Re-adjust the text container dimensions safely to prevent clipping
    enteredText.setPosition(layout->textAreaPosition.x - fontPadding,
                            layout->textAreaPosition.y,
                            layout->textAreaPosition.width + (2 * fontPadding),
                            layout->textAreaPosition.height);

    // 4. Force a repaint of the text area box
    enteredText.invalidate();
  }
}

void BamblooKeyboard2::setBufferPosition(uint16_t newPos) {
  bufferPosition = newPos;
  enteredText.invalidate();
}

void BamblooKeyboard2::setKeymappingList(
    const KeyMappingList *newKeyMappingList) {
  keyMappingList = newKeyMappingList;
  invalidate();
}

void BamblooKeyboard2::setupDrawChain(const Rect &invalidatedArea,
                                      Drawable **nextPreviousElement) {
  Container::setupDrawChain(invalidatedArea, nextPreviousElement);
  resetDrawChainCache();
  nextDrawChainElement = *nextPreviousElement;
  *nextPreviousElement = this;
}

void BamblooKeyboard2::handleDragEvent(const DragEvent &event) {
  if (highlightImage.isVisible() &&
      !highlightImage.getRect().intersect(event.getNewX(), event.getNewY())) {
    if (!cancelIsEmitted) {
      ClickEvent ce(ClickEvent::CANCEL, event.getOldX(), event.getOldY());
      handleClickEvent(ce);
    }
  }
}

void BamblooKeyboard2::draw(const Rect &invalidatedArea) const {
    if (!layout) {
        return;
    }

    auto font = FontManager::getFont(layout->keyFont);
    if (!font) {
        return;
    }

    LCD::StringVisuals stringVisuals;
    stringVisuals.font = font;
    stringVisuals.color.color = layout->keyFontColor.color;
    
    int height = font->getHeight();
    auto keyCursor = layout->keyArray;
    auto keyCursorEnd = layout->keyArray + layout->numberOfKeys;

    while(keyCursor < keyCursorEnd) {
        if (!keyCursor->keyArea.intersect(invalidatedArea)) {
            keyCursor++;
             continue;
        }
        
        keyCursor++;
    }
}

void BamblooKeyboard2::handleClickEvent(const ClickEvent &event) {
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
  }
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
        keyListener->execute(k.keyId);
      }
    }
  }

  highlightImage.setVisible(false);
  highlightImage.invalidate();
  if (eventType == ClickEvent::CANCEL) {
    cancelIsEmitted = true;
  }
}

bool BamblooKeyboard2::getCallbackAreaForCoordinates(CallbackArea *area,
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

bool BamblooKeyboard2::getKeyForCoordinates(Key *key, int16_t x,
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