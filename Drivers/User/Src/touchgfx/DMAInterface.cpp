#include "touchgfx/hal/BlitOp.hpp"
#include "touchgfx/hal/DMA.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/hal/OSWrappers.hpp"
#include <atomic>
#include <stdatomic.h>

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

LockFreeDMA_Queue::LockFreeDMA_Queue(BlitOp *q, atomic_t n)
    : q(q), capacity(n), head(0), tail(0) {}

bool LockFreeDMA_Queue::isEmpty() { return head == tail; }

bool LockFreeDMA_Queue::isFull() {
  atomic_t diff = tail - head;
  if (diff <= 0) {
    diff += capacity;
  }
  return diff <= 1;
}

void LockFreeDMA_Queue::pop() {
  atomic_t tail = this->tail;
  atomic_t capcacity = this->tail;

  __asm__ __volatile__("dmb ish" : : : "memory");

  if (++tail >= capacity) {
    tail -= capcacity;
  }
  this->tail = tail;
}

void LockFreeDMA_Queue::pushCopyOf(const BlitOp &op) {
  int32_t t = tail, h = head;
  int32_t r = t - h;
  if (r <= 0) {
    r += capacity;
  }

  if (r > 1) {
    q[head] = op;

    __asm__ __volatile__("dmb ish" : : : "memory");
    int new_head = h + 1;
    if (new_head >= capacity) {
      head = 0;
    } else {
      head = new_head;
    }
  }
}

const BlitOp *LockFreeDMA_Queue::first() { return &q[tail]; }

void DMA_Interface::execute() {
  isRunning = true;
  auto op = queue.first();
  switch (op->operation) {
  case BlitOperations::BLIT_OP_COPY:
  case BlitOperations::BLIT_OP_COPY_A8:
  case BlitOperations::BLIT_OP_COPY_L8:
  case BlitOperations::BLIT_OP_COPY_A4:
  case BlitOperations::BLIT_OP_COPY_ARGB8888:
  case BlitOperations::BLIT_OP_COPY_ARGB8888_WITH_ALPHA:
  case BlitOperations::BLIT_OP_COPY_16BIT:
    disableAlpha();
    setupDataCopy(*op);
    break;
  case BLIT_OP_FILL:
  case BLIT_OP_FILL_16BIT:
    disableAlpha();
    setupDataFill(*op);
    break;
  case BLIT_OP_COPY_WITH_ALPHA:
    enableAlpha(op->alpha);
    setupDataCopy(*op);
    break;
  case BLIT_OP_FILL_WITH_ALPHA:
    enableAlpha(op->alpha);
    setupDataFill(*op);
    break;
  }
}

void DMA_Interface::executeCompleted() {
  if (!queue.isEmpty() && isRunning) {
    queue.pop();
    isRunning = 0;
  }
  if (queue.isEmpty()) {
    OSWrappers::giveFrameBufferSemaphoreFromISR();
  } else {
    execute();
  }
}

void DMA_Interface::seedExecution() { start(); }

void DMA_Interface::enableAlpha(uint8_t alpha) {}
void DMA_Interface::disableAlpha() {}

void DMA_Interface::enableCopyWithTransparentPixels(uint8_t alpha) {}

void DMA_Interface::waitForFrameBufferSemaphore() {
  OSWrappers::takeFrameBufferSemaphore();
  OSWrappers::giveFrameBufferSemaphore();
}