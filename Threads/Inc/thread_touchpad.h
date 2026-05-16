#ifndef _THREAD_TOUCHPAD_H_
#define _THREAD_TOUCHPAD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uint8_t thread_touchpad_create(void);
uint8_t thread_touchpad_sample(int32_t *x, int32_t *y);

#ifdef __cplusplus
}
#endif
#endif