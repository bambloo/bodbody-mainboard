#ifndef _BODBODY_HELPER_H_
#define _BODBODY_HELPER_H_

#include "main.h"
#include "sqlite3.h"
#include "stm32h753xx.h"
#include "usart.h"

typedef struct {
  uint32_t id;
  char name[32];
  uint8_t gender;
  uint8_t age;
  uint8_t height;
  float weight;
} bodbody_user_info_t;

typedef struct {

} bodbody_user_data_t;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t bodbody_register(UART_HandleTypeDef *uart, IRQn_Type irq);
uint8_t bodbody_read_status();
uint8_t bodbody_save_user_info(sqlite3 *db, bodbody_user_info_t *info);
uint8_t bodbody_update_user_info(sqlite3 *db, bodbody_user_info_t *info);
int bodbody_count(sqlite3 *db, char *username);
int bodbody_load_user_info(sqlite3 *db, char *username,
                           bodbody_user_info_t *info, int skip, int count);

int bodbody_delete_user(sqlite3 *db, int user);
#ifdef __cplusplus
}
#endif

#endif