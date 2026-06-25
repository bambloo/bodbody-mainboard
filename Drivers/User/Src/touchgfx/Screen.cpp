#include "touchgfx/Screen.hpp"

using namespace touchgfx;

void Screen::draw() {
  Rect invalidatedRect(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
  draw(invalidatedRect);
}

void Screen::draw(Rect &rect) {
  Rect invalidatedArea = rect;
  invalidatedArea &= container.getRect();
  if (useSMOC) {
    startSMOC(invalidatedArea);
    return;
  }
  Rect absRect = container.getAbsoluteRect();
  invalidatedArea.x -= absRect.x;
  invalidatedArea.y -= absRect.y;
  container.draw(invalidatedArea);
}

void Screen::handleClickEvent(const ClickEvent &event) {
  if (focus && event.getType() == ClickEvent::PRESSED) {
    return;
  }
  Rect containerAbsRect = container.getAbsoluteRect();
  if (event.getType() == ClickEvent::PRESSED) {
    container.getLastChildNear(event.getX() - containerAbsRect.x,
                               event.getY() - containerAbsRect.y, &focus,
                               &fingerAdjustmentX, &fingerAdjustmentY);
  }
  if (focus) {
    Rect focusAbsRect = focus->getAbsoluteRect();

    int16_t localX =
        containerAbsRect.x - focusAbsRect.x + event.getX() + fingerAdjustmentX;
    int16_t localY =
        containerAbsRect.y - focusAbsRect.y + event.getY() + fingerAdjustmentY;
    ClickEvent newEvent(event.getType(), localX, localY);
    focus->handleClickEvent(newEvent);

    if (event.getType() != ClickEvent::PRESSED) {
      focus = nullptr;
      fingerAdjustmentX = 0;
      fingerAdjustmentY = 0;
    }
  }
}

void Screen::handleGestureEvent(const GestureEvent &event) {
  if (!focus) {
    return;
  }

  Rect absRec = focus->getAbsoluteRect();

  int16_t localX = event.getX() - absRec.x + fingerAdjustmentX;
  int16_t localY = event.getY() - absRec.y + fingerAdjustmentY;

  GestureEvent newEvent(event.getType(), event.getVelocity(), localX, localY);
  focus->handleGestureEvent(newEvent);
}

void Screen::handleDragEvent(const DragEvent &event) {
  if (!focus) {
    return;
  }
  Rect focusAbsRect = focus->getAbsoluteRect();

  int16_t oldX = event.getOldX() + fingerAdjustmentX;
  int16_t oldY = event.getOldY() + fingerAdjustmentY;

  bool oldInside = focusAbsRect.intersect(oldX, oldY);
  bool newInside = focusAbsRect.intersect(event.getNewX(), event.getNewY());

  if (oldInside && !newInside) {
    focus->handleClickEvent(
        ClickEvent(ClickEvent::ClickEventType::CANCEL, 0, 0));
  }

  Rect containerAbsRect = container.getAbsoluteRect();

  int16_t diffX = containerAbsRect.x - focusAbsRect.x;
  int16_t diffY = containerAbsRect.y - focusAbsRect.y;

  int16_t localOldX = diffX + event.getOldX() + this->fingerAdjustmentX;
  int16_t localOldY = diffY + event.getOldY() + this->fingerAdjustmentY;
  int16_t localNewX = diffX + event.getNewX() + this->fingerAdjustmentX;
  int16_t localNewY = diffY + event.getNewY() + this->fingerAdjustmentY;

  touchgfx::DragEvent localDragEvent(event.getType(), localOldX, localOldY,
                                     localNewX, localNewY);

  this->focus->handleDragEvent(localDragEvent);
}