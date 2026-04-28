#include "memory.h"

#include "is42s32200.h"
#include "thread_ltdc.h"
#include "thread_main.h"

#include "fmc.h"
#include "main.h"
#include "tx_api.h"

TX_THREAD main_thread;
void thread_main_entry(ULONG thread_input);

uint8_t thread_main_create(void) {
  is42s32200_init(&hsdram1);
  is42s32200_test();
  memory_pool_init();

  void *stack = malloc(DEFAULT_APP_STACK_SIZE);
  if (!stack) {
    return TX_NO_MEMORY;
  }
  return tx_thread_create(&main_thread, "TX MAIN", thread_main_entry, 0, stack,
                          DEFAULT_APP_STACK_SIZE, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
}

void thread_main_entry(ULONG thread_input) {

  thread_ltdc_create();
  while (1) {
    tx_thread_sleep(1000);
    tx_thread_relinquish();
  }
}