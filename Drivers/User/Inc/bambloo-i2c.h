#ifndef _SW_I2C_H_
#define _SW_I2C_H_

#include "main.h"

HAL_StatusTypeDef Bambloo_I2C_Init(void);
HAL_StatusTypeDef Bambloo_I2C_Mem_Read(uint16_t dev_addr, uint32_t mem_addr,
                                  uint8_t *data, int size, int addr_len);
HAL_StatusTypeDef Bambloo_I2C_Mem_Write(uint16_t dev_addr, uint32_t mem_addr,
                                   uint8_t *data, int size, int addr_len);
#endif