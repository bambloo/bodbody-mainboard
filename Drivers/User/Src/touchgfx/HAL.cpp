#include "touchgfx/HAL/hal.hpp"
#include "touchgfx/hal/GPIO.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/hal/OSWrappers.hpp"
#include "touchgfx/hal/Types.hpp"

using namespace touchgfx;

uint16_t HAL::DISPLAY_WIDTH;
uint16_t HAL::DISPLAY_HEIGHT;
uint16_t HAL::FRAME_BUFFER_WIDTH;
uint16_t HAL::FRAME_BUFFER_HEIGHT;

DisplayRotation HAL::DISPLAY_ROTATION;

bool HAL::USE_DOUBLE_BUFFERING;
bool HAL::USE_ANIMATION_STORAGE;

bool HAL::isDrawing;

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

bool HAL::beginFrame() { return !(USE_DOUBLE_BUFFERING && swapRequested); }

// void HAL::tick() {
//   if (!beginFrame()) {
//     goto end;
//   }

//   endFrame();
//   if (refreshStrategy != REFRESH_STRATEGY_PARTIAL_BUFFER_TFT_CTRL) {
//     GPIO::clear(GPIO::GPIO_ID::RENDER_TIME);
//   }
//   vSyncForFrame = vSyncCnt;
//   frameBufferUpdatedThisFrame = 0;

//   if (displayOrientationChangeRequested) {
//     performDisplayOrientationChange();
//     displayOrientationChangeRequested = 0;
//   }

//   isDrawing = 1;
// end;
//   isDrawing = 0;
// }