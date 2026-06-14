#include "memory.h"
#include "main.h"
#include "tx_api.h"
#include <stdlib.h>

__ALIGN_BEGIN static UCHAR
    glb_d1_byte_pool_buffer[TX_D1_MEM_POOL_SIZE] __ALIGN_END
    __attribute__((section(".d1ram")));
static TX_BYTE_POOL glb_d1_byte_pool;

__ALIGN_BEGIN static UCHAR glb_dma_pool_buffer[0x40000] __ALIGN_END
    __attribute__((section(".d2ram")));

static TX_BYTE_POOL glb_dma_byte_pool;

void memory_pool_init() {
  if (tx_byte_pool_create(&glb_d1_byte_pool, "d2 memory pool",
                          glb_d1_byte_pool_buffer, TX_D1_MEM_POOL_SIZE)) {
    do {
    } while (1);
  }

  if (tx_byte_pool_create(&glb_dma_byte_pool, "dma memory pool",
                          glb_dma_pool_buffer, 0x40000)) {
    do {
    } while (1);
  }
}

void *memory_pool_cache_free() { return &glb_dma_byte_pool; }
void *memory_pool_d1() { return &glb_d1_byte_pool; }

// #define MEM_RECORD_TABLE_COUNT 128
// uint32_t mem_table[MEM_RECORD_TABLE_COUNT][2];
// uint32_t mem_index = 0;
void *malloc(size_t size) {
  void *ptr = NULL;
  // for (int i = 0; i < MEM_RECORD_TABLE_COUNT; i++) {
  //   if (!mem_table[i][0]) {
  //     continue;
  //   }
  //   if (0xabcdef11 !=
  //       *(uint32_t *)((uint8_t *)mem_table[i][0] + mem_table[i][1])) {
  //     while (1) {
  //       continue;
  //     }
  //   }
  // }
  if (tx_byte_allocate(&glb_d1_byte_pool, &ptr, size + 8, TX_NO_WAIT)) {
    do {
    } while (1);
  }
  
  // while (mem_table[mem_index][0]) {
  //   ++mem_index;
  //   mem_index = mem_index & (MEM_RECORD_TABLE_COUNT - 1);
  // }
  // mem_table[mem_index][0] = (uint32_t)ptr;
  // mem_table[mem_index][1] = size;
  // *(uint32_t *)((uint8_t *)ptr + size) = 0xabcdef11;
  // if ((uint32_t)ptr == 0x24035d68) {
  //   while(1) {{
  //     return ptr;
  //   }}
  // }
  return ptr;
}

void free(void *buffer) {
  // if ((uint32_t)buffer == 0x24035d68) {
  //   int i = 1;
  //   i += 1;
  //   buffer += i;
  //   buffer -= i;
  // }
  // for (int i = 0; i < MEM_RECORD_TABLE_COUNT; i++) {
  //   if (!mem_table[i][0]) {
  //     continue;
  //   }
  //   if (0xabcdef11 !=
  //       *(uint32_t *)((uint8_t *)mem_table[i][0] + mem_table[i][1])) {
  //     while (1) {
  //       continue;
  //     }
  //   }
  //   if (mem_table[i][0] == (uint32_t)buffer) {
  //     mem_table[i][0] = 0;
  //     mem_table[i][1] = 0;
  //   }
  // }
  tx_byte_release(buffer);
}

void* realloc(void * p, unsigned int size) {
  void * mem = malloc(size);
  memcpy(mem, p, size);
  free(p);
  return mem;
}