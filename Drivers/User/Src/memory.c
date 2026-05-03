#include "memory.h"
#include "main.h"
#include "tx_api.h"

__ALIGN_BEGIN static UCHAR glb_d1_byte_pool_buffer[TX_D1_MEM_POOL_SIZE] __ALIGN_END
    __attribute__((section(".d1ram")));
static TX_BYTE_POOL glb_d1_byte_pool;

__ALIGN_BEGIN static UCHAR glb_dma_pool_buffer[0x40000] __ALIGN_END __attribute__((section(".d2ram")));

static TX_BYTE_POOL glb_dma_byte_pool;

void memory_pool_init() {
  if (tx_byte_pool_create(&glb_d1_byte_pool, "d2 memory pool", glb_d1_byte_pool_buffer,
                          TX_D1_MEM_POOL_SIZE)) {
    do {
    } while (1);
  }

  if (tx_byte_pool_create(&glb_dma_byte_pool, "dma memory pool", glb_dma_pool_buffer,
                          TX_D1_MEM_POOL_SIZE)) {
    do {
    } while (1);
  }
}

void* memory_pool_dma() {
  return &glb_dma_byte_pool;
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