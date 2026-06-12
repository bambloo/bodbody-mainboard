#ifndef BAMBLOOKEYBOARDBASE_HPP
#define BAMBLOOKEYBOARDBASE_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/TypedText.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/events/ClickEvent.hpp>
#include <touchgfx/events/DragEvent.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

using namespace touchgfx;

namespace bambloo {
class BamblooKeyboardBase : public touchgfx::Container {
public:
  struct __attribute__((aligned(4))) Key {
    uint32_t keyId;             ///< The id of a key
    Rect keyArea;               ///< The area occupied by the key
    BitmapId highlightBitmapId; ///< A bitmap to show when the area is "pressed"
  };

  struct CallbackArea {
    ///< The area occupied by a key
    Rect keyArea;

    ///< The callback to execute, when the area is "pressed". The
    ///< callback should be a Callback<YourClass> member in the
    ///< class using the keyboard
    GenericCallback<> *callback;

    ///< A bitmap to show when the area is "pressed"
    BitmapId highlightBitmapId;
  };

  struct Layout {
    BitmapId bitmap;      ///< The bitmap used for the keyboard layout
    const Key *keyArray;  ///< The keys on the keyboard layout
    uint8_t numberOfKeys; ///< The number of keys on this keyboard layout
    CallbackArea
        *callbackAreaArray; ///< The array of areas and corresponding callbacks
    uint8_t numberOfCallbackAreas; ///< The number of areas and corresponding
                                   ///< callbacks
    Rect textAreaPosition;         ///< The area where text is written
    TypedText textAreaFont;        ///< The font used for typing text
    colortype textAreaFontColor;   ///< The color used for the typing text
    FontId keyFont;                ///< The font used for the keys
    colortype keyFontColor;        ///< The color used for the keys
  };

  BamblooKeyboardBase();

  void setBuffer(Unicode::UnicodeChar *newBuffer, uint16_t newBufferSize);
  void setLayout(const Layout *newLayout);
  void setTextIndentation();
  const Layout *getLayout() const { return layout; }

  void setKeymappingList(const Unicode::UnicodeChar *newKeyMappingList);

  void setBufferPosition(uint16_t newPos);

  uint16_t getBufferPosition() { return bufferPosition; }
  Unicode::UnicodeChar *getBuffer() const { return buffer; }

  virtual void draw(const Rect &invalidatedArea) const;

  virtual void handleClickEvent(const ClickEvent &event);

  virtual void handleDragEvent(const DragEvent &event);

  void setKeyListener(GenericCallback<Unicode::UnicodeChar> &callback) {
    keyListener = &callback;
  }

protected:
  GenericCallback<Unicode::UnicodeChar>
      *keyListener; ///< Pointer to callback being executed when a key is
                    ///< pressed.

  Unicode::UnicodeChar *buffer; ///< Pointer to null-terminated buffer where the
                                ///< entered text is being displayed.
  uint16_t bufferSize;          ///< Size of the buffer
  uint16_t bufferPosition;      ///< Current position in buffer.
  Image image;                  ///< Layout bitmap.
  TextAreaWithOneWildcard
      enteredText; ///< Widget capable of displaying the entered text buffer.
  const Layout *layout;                       ///< Pointer to layout.
  const Unicode::UnicodeChar *keyMappingList; ///< Pointer to key mapping.
  Image highlightImage; ///< Image to display when a key is highlighted.
  bool cancelIsEmitted; ///< Tells if a cancel is emitted to check when a key is
  ///< released

  bool getKeyForCoordinates(Key *, int16_t x, int16_t y) const;
  bool getCallbackAreaForCoordinates(CallbackArea *, int16_t x,
                                     int16_t y) const;

  virtual void setupDrawChain(const Rect &invalidatedArea,
                              Drawable **nextPreviousElement);
};
} // namespace touchgfx

#endif // BAMBLOOKEYBOARD2_HPP
