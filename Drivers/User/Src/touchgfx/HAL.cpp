#include "touchgfx/HAL/hal.hpp"
#include "touchgfx/Bitmap.hpp"
#include "touchgfx/hal/BlitOp.hpp"
#include "touchgfx/hal/GPIO.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/hal/OSWrappers.hpp"
#include "touchgfx/hal/Types.hpp"
#include <cstdint>

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
//     latTouched = false;
//   }
// }

void HAL::allowDMATransfers() {
  dma.setAllowed(true);
  dma.start();
}

void HAL::flushDMA() { dma.flush(); }

uint16_t *HAL::lockFrameBuffer() {
  if (!USE_DOUBLE_BUFFERING && refreshStrategy == REFRESH_STRATEGY_DEFAULT &&
      dma.isDmaQueueEmpty()) {
    while (!dma.getAllowed()) {
    }
  }
  OSWrappers::takeFrameBufferSemaphore();
  setRenderingMethod(RenderingMethod::SOFTWARE);
  return getClientFrameBuffer();
}

void HAL::unlockFrameBuffer() {
  OSWrappers::giveFrameBufferSemaphore();
  dma.setReserved(true);
}

void HAL::blitCopy(const uint16_t *pSrc, const uint8_t *pClut, uint16_t x,
                   uint16_t y, uint16_t width, uint16_t height,
                   uint16_t srcWidth, uint8_t alpha, bool hasTransparentPixels,
                   uint16_t dstWidth, Bitmap::BitmapFormat srcFormat,
                   Bitmap::BitmapFormat dstFormat, bool replaceBgAlpha) {
  BlitOp dop;
  if (srcFormat == Bitmap::BitmapFormat::L8) {
    dop.operation = BlitOperations::BLIT_OP_COPY_L8;
  } else if (srcFormat == Bitmap::BitmapFormat::ARGB8888) {
    if (alpha != 0xff || hasTransparentPixels) {
      dop.operation = BLIT_OP_COPY_ARGB8888_WITH_ALPHA;
    } else {
      dop.operation = BLIT_OP_COPY_ARGB8888;
    }
  } else {
    if (alpha != 0xff || hasTransparentPixels) {
      dop.operation = BLIT_OP_COPY_WITH_ALPHA;
    } else {
      dop.operation = BLIT_OP_COPY;
    }
  }

  dop.pClut = pClut;
  dop.alpha = alpha;
  dop.nSteps = width;
  dop.nLoops = height;
  dop.pSrc = pSrc;
  dop.srcLoopStride = srcWidth;
  auto cfb = getClientFrameBuffer();
  dop.pDst = getDstAddress(x, y, cfb, dstWidth, dstFormat);
  dop.dstLoopStride = dstWidth;
  dop.srcFormat = srcFormat;
  dop.dstFormat = dstFormat;
  dop.replaceBgAlpha = replaceBgAlpha;
  dop.replaceFgAlpha = false;
  dma.addToQueue(dop);
}

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