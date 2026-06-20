#ifndef _BODBODY_HELPER_H_
#define _BODBODY_HELPER_H_

#include "main.h"
#include "stm32h753xx.h"
#include "usart.h"

uint8_t bodbody_register(UART_HandleTypeDef *uart, IRQn_Type irq);
uint8_t bodbody_read_status();

#endif