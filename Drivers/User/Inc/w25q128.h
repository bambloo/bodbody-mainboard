#ifndef _W25Q128_H_
#define _W25Q128_H_

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_qspi.h"
#include "stdint.h"

HAL_StatusTypeDef w25_load_sr(uint8_t index, uint8_t *sr);
HAL_StatusTypeDef w25_read_unique_id(uint64_t *id);
HAL_StatusTypeDef w25_read_jedec_id(uint64_t *id);
HAL_StatusTypeDef w25_write(uint32_t addr, uint8_t *buf, int len);
HAL_StatusTypeDef w25_read(uint32_t addr, uint8_t *buf, int len);
HAL_StatusTypeDef w25_map();
HAL_StatusTypeDef w25_unmap();

#endif