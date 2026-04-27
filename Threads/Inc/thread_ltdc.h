#ifndef _THREAD_LTDC_H_
#define _THREAD_LTDC_H_ 

#include "stdint.h"

#define LTDC_COLS 800
#define LTDC_ROWS 600
#define LTDC_BYTES_PER_PIXEL 3
#define LTDC_SIZE (LTDC_COLS * LTDC_ROWS * LTDC_BYTES_PER_PIXEL)

uint8_t thread_ltdc_create(void);

#endif