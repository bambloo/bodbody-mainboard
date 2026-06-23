#include "touchgfx/hal/BlitOp.hpp"
#include "touchgfx/hal/DMA.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/hal/OSWrappers.hpp"

using namespace touchgfx;

void DMA_Interface::addToQueue(const BlitOp &op) {
  while (queue.isFull())
    ;
  auto hal = HAL::getInstance();
  hal->disableInterrupts();
  OSWrappers::tryTakeFrameBufferSemaphore();
  hal->setRenderingMethod(HAL::RenderingMethod::SOFTWARE);
  queue.pushCopyOf(op);
  seedExecution();
  hal->enableInterrupts();
}

void DMA_Interface::start() {
  if (!isAllowed || isRunning || queue.isEmpty()) {
    return;
  }
  execute();
}