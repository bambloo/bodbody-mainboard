#include "app_ltdc.h"
#include "dma2d.h"
#include "ltdc.h"
#include "main.h"
#include "rng.h"
#include "stdlib.h"
#include "tx_api.h"

TX_THREAD ltdc_thread;
TX_EVENT_FLAGS_GROUP ltdc_efg;

__ALIGN_BEGIN static uint8_t ltdc_memory[LTDC_WIDTH * LTDC_HEIGHT * 3] __ALIGN_END __attribute__((section(".sdram")));
__ALIGN_BEGIN static uint8_t ltdc_canvas[LTDC_WIDTH * LTDC_HEIGHT * 3] __ALIGN_END __attribute__((section(".sdram")));
void app_ltdc_entry(ULONG thread_input);

uint8_t app_ltdc_create(void) {
  UINT ret = TX_SUCCESS;
  void *stack = malloc(DEFAULT_APP_STACK_SIZE);
  if (!stack) {
    return TX_NO_MEMORY;
  }
  ret = tx_thread_create(&ltdc_thread, "LTDC", app_ltdc_entry, 0, stack,
                         DEFAULT_APP_STACK_SIZE, 10, 10, TX_NO_TIME_SLICE,
                         TX_AUTO_START);

  return ret;
}

void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *ltdc) {
  tx_event_flags_set(&ltdc_efg, 1, TX_OR);
}

static void draw_pixel(int16_t x, int16_t y, uint32_t color) {
  if (y < 0 || x < 0 || x > 800 || y > 600) {
    return;
  }
  ltdc_canvas[(y * 800 + x) * 3 + 0] = color >> 0x00;
  ltdc_canvas[(y * 800 + x) * 3 + 1] = color >> 0x08;
  ltdc_canvas[(y * 800 + x) * 3 + 2] = color >> 0x10;
}

static void draw_circle(uint16_t x0, uint16_t y0, uint16_t radius,
                        uint16_t color) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    draw_pixel(x0 + x, y0 + y, color);
    draw_pixel(x0 + y, y0 + x, color);
    draw_pixel(x0 - y, y0 + x, color);
    draw_pixel(x0 - x, y0 + y, color);
    draw_pixel(x0 - x, y0 - y, color);
    draw_pixel(x0 - y, y0 - x, color);
    draw_pixel(x0 + y, y0 - x, color);
    draw_pixel(x0 + x, y0 - y, color);

    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}

void draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  uint16_t i;

  // ??x1 <= x2
  if (x1 > x2) {
    uint16_t temp = x1;
    x1 = x2;
    x2 = temp;
  }

  // ???(x1,y1)?(x2,y1)????
  for (i = x1; i <= x2; i++) {
    draw_pixel(i, y1, color);
  }
}

static void fill_circle(int16_t x0, int16_t y0, int16_t radius,
                        uint16_t color) {
  draw_line(x0 - radius + 1, y0, x0 + radius - 1, y0, color);
  int t = radius;
  for (int i = 0; i < radius; i++) {
    while (t > 0 && i * i + t * t > radius * radius) {
      t--;
    }
    draw_line(x0 - t, y0 + i, x0 + t, y0 + i, 0xFF);
    draw_line(x0 - t, y0 - i, x0 + t, y0 - i, 0xFF);
  }
}

// static void draw_tline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
//                        uint16_t color) {
//   int16_t dx, dy;  // ????
//   int16_t sx, sy;  // ????
//   int16_t err, e2; // ???

//   // ??????
//   dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
//   dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);

//   // ??????
//   sx = (x2 > x1) ? 1 : -1;
//   sy = (y2 > y1) ? 1 : -1;

//   // ??????
//   err = ((dx > dy) ? dx : -dy) / 2;

//   while (1) {
//     // ??????
//     fill_circle(x1, y1, 8, color);

//     // ???????
//     if (x1 == x2 && y1 == y2)
//       break;

//     e2 = err;

//     // x????
//     if (e2 > -dx) {
//       err -= dy;
//       x1 += sx;
//     }

//     // y????
//     if (e2 < dy) {
//       err += dx;
//       y1 += sy;
//     }
//   }
// }

void app_ltdc_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
  draw_circle(x, y, r, color);
  HAL_LTDC_ProgramLineEvent(&hltdc, 500);
}

void app_ltdc_fill_circle(int x, int y, int r, int color) {
  fill_circle(x, y, r, color);
  HAL_LTDC_ProgramLineEvent(&hltdc, 500);
}

void app_ltdc_entry(ULONG thread_input) {
  memset(ltdc_canvas, 0x3F, LTDC_BUFSIZE);
  memset(ltdc_memory, 0, LTDC_BUFSIZE);
  tx_event_flags_create(&ltdc_efg, 0);

  __HAL_LTDC_ENABLE_IT(&hltdc, LTDC_IT_LI);

  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
  tx_thread_sleep(100);
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_EN_GPIO_Port, LED_EN_Pin, GPIO_PIN_SET);

  tx_event_flags_set(&ltdc_efg, 0x00, TX_AND);
  uint32_t seed;

  while (1) {
    ULONG flags;
    HAL_RNG_GenerateRandomNumber(&hrng, &seed);
    tx_event_flags_get(&ltdc_efg, 1, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);

    HAL_DMA2D_Start(&hdma2d, (uint32_t)ltdc_canvas, (uint32_t)ltdc_memory, 800,
                    600);
    HAL_DMA2D_PollForTransfer(&hdma2d, 1000);
  }
}
