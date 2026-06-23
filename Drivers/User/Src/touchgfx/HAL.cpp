#include "touchgfx/HAL/hal.hpp"
#include "touchgfx/hal/GPIO.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/hal/OSWrappers.hpp"

using namespace touchgfx;

HAL *HAL::instance;

// void HAL::noTouch() {
//   if (lastTouched) {
//     gestures.registerClickEvent(ClickEvent::ClickEventType::RELEASED, lastX,
//                                 lastY);
//     lastTouched = false;
//   }
// }

void HAL::taskEntry() {
  enableLCDControllerInterrupt();
  enableInterrupts();

  while (1) {
    if (refreshStrategy != REFRESH_STRATEGY_PARTIAL_BUFFER_TFT_CTRL) {
      OSWrappers::waitForVSync();
    }
    backPorchExited();
  }
}

void HAL::swapFrameBuffers() {
  if (swapRequested) {
    GPIO::toggle(GPIO::GPIO_ID::FRAME_RATE);
    swapRequested = 0;
    clientDirty = 0;

    auto clientFrameBuffer = getClientFrameBuffer();
    setTFTFrameBuffer(clientFrameBuffer);
    if (frameBufferListener) {
      frameBufferListener->setFrameBuffer((uint8_t *)clientFrameBuffer,
                                          FRAME_BUFFER_WIDTH);
    }
  }
}