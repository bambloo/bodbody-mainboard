#include "touchgfx/Application.hpp"
#include "string.h"
#include "touchgfx/Screen.hpp"
#include "touchgfx/hal/HAL.hpp"
#include "touchgfx/transforms/DisplayTransformation.hpp"

using namespace touchgfx;

Application *touchgfx::Application::instance;
Screen *Application::currentScreen;

int rects = 0;
touchgfx::Application *touchgfx::Application::getInstance() {
  return touchgfx::Application::instance;
}

Application::Application() {
  transitionHandled = false;
  redraw = {0, 0, 0, 0};
  rects = 0;

  instance = 0;
  currentScreen = 0;
  currentTransition = 0;
  timerWidgets.clear();
}

void Application::handleGestureEvent(const GestureEvent &event) {
  if (currentScreen) {
    currentScreen->handleGestureEvent(event);
  }
}

void Application::handleDragEvent(const DragEvent &event) {
  if (currentScreen) {
    currentScreen->handleDragEvent(event);
  }
}

void Application::handleClickEvent(const ClickEvent &event) {
  if (currentScreen) {
    currentScreen->handleClickEvent(event);
  }
}

void Application::handleKeyEvent(uint8_t key) {
  if (currentScreen) {
    currentScreen->handleKeyEvent(key);
  }
}

void Application::switchScreen(Screen *newScreen) {
  if (currentScreen) {
    currentScreen->tearDownScreen();
  }
  timerWidgets.clear();
  currentScreen = newScreen;
  currentScreen->setupScreen();
  draw();
}

void Application::requestRedraw() {
  redraw = {0, 0, (int16_t)HAL::DISPLAY_WIDTH, (int16_t)HAL::DISPLAY_HEIGHT};
}

void Application::clearCachedAreas() { cachedDirtyAreas.clear(); }

void Application::invalidate() {
  Rect invalidatedArea = {0, 0, (int16_t)HAL::DISPLAY_WIDTH,
                          (int16_t)HAL::DISPLAY_HEIGHT};
  invalidateArea(invalidatedArea);
}

void Application::invalidateArea(Rect rect) {}

// static inline int16_t waitForScanPassV(uint16_t min_dirty, int16_t left_boundry) {
//   int currentTftLineX;
//   while (true) {
//     currentTftLineX =
//         HAL::DISPLAY_WIDTH - HAL::getInstance()->getTFTCurrentLine();
//     if (currentTftLineX <= (left_boundry - safety_gap) ||
//         currentTftLineX <= min_dirty) {
//       break;
//     }

//     if (HAL::getInstance()) {
//     }
//   }

//   return currentTftLineX;
// }

// void Application::drawCachedAreas() {
//   if (!redraw.isEmpty()) {
//     invalidateArea(redraw);
//     redraw = {0, 0, 0, 0};
//   }

//   if (debugPrinter && !debugRegionInvalidRect.isEmpty()) {
//     touchgfx::DisplayTransformation::transformDisplayToFrameBuffer(
//         debugRegionInvalidRect);
//     invalidateArea(debugRegionInvalidRect);
//     debugRegionInvalidRect = {0, 0, 0, 0};
//   }

//   if (!cachedDirtyAreas.size() &&
//       HAL::getInstance()->getFrameRefreshStrategy() !=
//           HAL::REFRESH_STRATEGY_PARTIAL_BUFFER_TFT_CTRL) {
//     return;
//   }

//   RectVector_t backupDirtyAreas;
//   for (int i = 0; i < cachedDirtyAreas.size(); i++) {
//     backupDirtyAreas.add(cachedDirtyAreas[i]);
//   }

//   for (int i = 0; i < lastRects.size(); i++) {
//     invalidateArea(lastRects[i]);
//   }

//   if (HAL::getInstance()->getFrameRefreshStrategy() ==
//       HAL::REFRESH_STRATEGY_OPTIM_SINGLE_BUFFER_TFT_CTRL) {

//     if (HAL::DISPLAY_ROTATION == ORIENTATION_PORTAIT) {
//       Rect refresh_boundary;
//       refresh_boundary.x = HAL::DISPLAY_WIDTH;
//       for (int i = 0; i < cachedDirtyAreas.size(); i++) {
//         if (cachedDirtyAreas[i].x < minDirtyX) {
//           refresh_boundary.x = cachedDirtyAreas[i].x;
//         }
//       }

//       int safety_gap = 1;
//       if (HAL::DISPLAY_WIDTH > 0x0f) {
//         safety_gap = HAL::DISPLAY_WIDTH >> 3;
//       }

//       while (true) {
//         uint16_t left_boundry = refresh_boundary.x;
//         if (refresh_boundary.x <= HAL::DISPLAY_WIDTH) {
//           break;
//         }

//         uint16_t currentTftLineX;
//       }
//     }
//   }
// }