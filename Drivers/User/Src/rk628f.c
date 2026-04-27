#include "rk628f.h"
#include "tx_api.h"

HAL_StatusTypeDef rk628f_test() {
  HAL_StatusTypeDef status = HAL_OK;

  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
  tx_thread_sleep(100);
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_EN_GPIO_Port, LED_EN_Pin, GPIO_PIN_SET);
  tx_thread_sleep(100);
  uint8_t regs[16];
  uint8_t hdmi_regs[16];

  (void)hdmi_regs;
  while (1) {
    status = Bambloo_I2C_Mem_Read(0xA0, 0xe0000700, regs, 4, 4);
    // status = Bambloo_I2C_Mem_Write(0xA0, 0x00000000, regs, 4, 4);
    tx_thread_sleep(1000);
  }
  return status;
}