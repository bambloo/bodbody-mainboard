#include "bodbody-helper.h"
#include "stm32h7xx_hal_def.h"
#include "uart-helper.h"

uart_helper_t bodbody_uart_helper;

void bodbody_data_clbk(uart_helper_t *helper) {
  
}

uint8_t bodbody_helper_register(UART_TypeDef *uart, IRQn_Type irq) {
  return uart_helper_register(&bodbody_uart_helper, uart, bodbody_data_clbk,
                              irq);
}