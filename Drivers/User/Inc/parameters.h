#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "main.h"

#define PARAMETERS_MAGIC 0xBABAFEFE
#define PARAMETERS_VERSION 0x00000001

typedef struct {
  uint32_t magic;
  uint32_t version;
  uint8_t pin_sha1[20];
} parameters_t;


#if defined(__cplusplus)
extern "C" {
#endif
parameters_t *parameters_get();

#if defined(__cplusplus)
}
#endif

#endif