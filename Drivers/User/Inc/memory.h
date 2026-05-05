#ifndef _MEMORY_H_
#define _MEMORY_H_

#define TX_D1_MEM_POOL_SIZE 0x80000

void memory_pool_init();
void *memory_pool_cache_free();

#endif