#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>

#define htons(x) (unsigned short)((((unsigned short)x) << 8) | (((unsigned short)x) >> 8))
#define htonl(x)                                                                                                       \
  ((((x) >> 24) & 0x000000FF) | (((x) >> 8) & 0x0000FF00) | (((x) << 8) & 0x00FF0000) | (((x) << 24) & 0xFF000000))

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define SWAP(a, b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))

static inline void f2s(char *c, float v) {
  int32_t integer = (int32_t)v;
  int32_t decimal = (int32_t)((v - integer) * 10.0f);

  sprintf(c, "%ld.%01ld", integer, decimal);
}
#endif