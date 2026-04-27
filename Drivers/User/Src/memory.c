#include "memory.h"
#include "tx_api.h"

__ALIGN_BEGIN static UCHAR
    glb_d1_byte_pool_buffer[TX_D1_MEM_POOL_SIZE] __ALIGN_END
    __attribute__((section(".data_ram_d1")));
static TX_BYTE_POOL glb_d1_byte_pool;

void memory_pool_init() {
  if (tx_byte_pool_create(&glb_d1_byte_pool, "d1 memory pool",
                          glb_d1_byte_pool_buffer, TX_D2_MEM_POOL_SIZE)) {
    do {
    } while (1);
  }
}

void *malloc(size_t size) {
  void *ptr = NULL;

  if (tx_byte_allocate(&glb_d1_byte_pool, &ptr, size, TX_NO_WAIT)) {
    do {
    } while (1);
  }
  return ptr;
}

void free(void *buffer) { tx_byte_release(buffer); }