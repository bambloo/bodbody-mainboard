#ifndef _IS42S32200_H_
#define _IS42S32200_H_

#include "stm32h7xx_hal.h"
#include "fmc.h"

#define IS32S32200_DEFAULT_TIMEOUT 1000

HAL_StatusTypeDef is42s32200_init(SDRAM_HandleTypeDef *hsdram);
HAL_StatusTypeDef is42s32200_test();

#endif