#include "touchgfx/widgets/TextArea.hpp"
#include "touchgfx/TypedText.hpp"

using namespace touchgfx;

// void TextArea::setBaselineY(int16_t y) {
//   TypedText currentText = getTypedText();
//   assert(TypedTexts::typedTexts);
//   assert(currentText.hasValidId());

//   auto font = currentText.getFont();

//   int16_t off = textFont->getFontHeight();
//   int16_t top = y - off;
//   setY(y - off);
// }