#include "memory.h"

#include "is42s32200.h"
#include "stm32h753xx.h"
#include "stm32h7xx_hal_sd.h"
#include "thread_ltdc.h"
#include "thread_main.h"
#include "thread_touchpad.h"

#include "bodbody-helper.h"
#include "fmc.h"
#include "main.h"
#include "sdmmc.h"
#include "tx_api.h"
#include "uart-helper.h"
#include "w25q128.h"

TX_THREAD main_thread;
void thread_main_entry(ULONG thread_input);

uint8_t thread_main_create(void) {
  uint32_t jedec_id;
  w25_read_jedec_id(&jedec_id);
  // is42s32200_test();
  
  memory_pool_init();
  thread_ltdc_create();
  thread_touchpad_create();

  bodbody_register(&huart1, DMA1_Stream0_IRQn);

  void *stack = malloc(DEFAULT_APP_STACK_SIZE);
  if (!stack) {
    return TX_NO_MEMORY;
  }
  return tx_thread_create(&main_thread, "TX MAIN", thread_main_entry, 0, stack,
                          DEFAULT_APP_STACK_SIZE, 10, 10, TX_NO_TIME_SLICE,
                          TX_AUTO_START);
}

void thread_main_entry(ULONG thread_input) {
  while (1) {
    _tx_thread_sleep(10);
    uart_helper_check();
  }
}