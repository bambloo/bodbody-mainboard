#include "bodbody-helper.h"
#include "stm32h753xx.h"
#include "stm32h7xx_hal_def.h"
#include "tx_api.h"
#include "uart-helper.h"

typedef struct {
  uart_helper_t uh;

  uint8_t cm_len;
  uint8_t cm_stg;
  uint8_t cm_buf[BUF_SIZE];

  TX_EVENT_FLAGS_GROUP efg;
} body_helper_t;

enum {
  BODBODY_COMMAND_STATUS_HEAD,
  BODBODY_COMMAND_STATUS_LENG,
  BODBODY_COMMAND_STATUS_BODY,
};

static body_helper_t body_helper __attribute__((section(".d2ram")));

void bodbody_cmmd_clbk(body_helper_t *helper) {
  tx_event_flags_set(&bhelper.efg, 0x01, TX_OR);
}

uint8_t bodbody_push_data(uart_helper_t *helper, uint8_t data, uint8_t pck) {
  helper->tx_buf[helper->tx_csr++] = data;
  return pck + data;
}

uint8_t bodbody_sdrc_command(body_helper_t *bh, uint8_t type, uint8_t cmmd,
                             uint8_t *tdata, uint8_t len) {

  uart_helper_t *helper = &bh->uh;
  uint8_t tlen = len + 4;
  helper->tx_csr = 2;
  helper->tx_buf[0] = 0xC5;
  helper->tx_buf[1] = tlen;
  helper->tx_csr = 2;

  uint8_t tmp = bodbody_push_data(helper, type, 0);
  tmp = bodbody_push_data(helper, cmmd, tmp);

  for (int i = 0; i < len; i++) {
    tmp = bodbody_push_data(helper, tdata[i], tmp);
  }
  helper->tx_buf[helper->tx_csr++] = tmp;
  helper->tx_buf[helper->tx_csr++] = 0xBB;

  ULONG flags;
  uint8_t cnt = 0;
  do {
    if (++cnt > 3) {
      return TX_WAIT_ERROR;
    }
    tmp = uart_helper_send(helper, helper->tx_buf, helper->tx_csr);
  } while (tx_event_flags_get(&bh->efg, 0x01, TX_AND_CLEAR, &flags, 100) !=
           TX_SUCCESS);

  return TX_SUCCESS;
}

void body_data_clbk(body_helper_t *helper) {
  uart_helper_t *uh = &body_helper.uh;
  while (uart_helper_available(uh)) {
    switch (helper->cm_stg) {
    case BODBODY_COMMAND_STATUS_HEAD: {
      uint8_t data = uart_helper_read_byte(uh);
      if (data == 0x5C) {
        helper->cm_stg = BODBODY_COMMAND_STATUS_LENG;
      }
      break;
    }
    case BODBODY_COMMAND_STATUS_LENG: {
      helper->cm_len = uart_helper_read_byte(uh);
      if (helper->cm_len > 2) {
        helper->cm_stg = BODBODY_COMMAND_STATUS_HEAD;
      }
      break;
    }
    case BODBODY_COMMAND_STATUS_BODY: {
      if (uart_helper_available(uh) < helper->cm_len) {
        return;
      }
      uint8_t check = 0;
      uint8_t data_len = helper->cm_len - 2;

      for (int i = 0; i < data_len; i++) {
        helper->cm_buf[i] = uart_helper_read_byte(uh);
        check += helper->cm_buf[i];
      }

      if (check == uart_helper_read_byte(uh) &&
          0xbb == uart_helper_read_byte(uh)) {
        helper->cm_len = data_len;
        bodbody_cmmd_clbk(helper);
      }
      helper->cm_stg = BODBODY_COMMAND_STATUS_HEAD;
      break;
    }
    }
  }
}

uint8_t bodbody_read_status() {
  uint8_t enc = 0;
  uint8_t res = bodbody_sdrc_command(&body_helper, 0x01, 0xC0, &enc, 1);
  return 0;
}

uint8_t bodbody_helper_register(UART_HandleTypeDef *uart, IRQn_Type irq) {
  uint8_t res = uart_helper_register(&body_helper.uh, uart,
                                     (uart_data_clbk_t)body_data_clbk, irq);

  tx_event_flags_create(&body_helper.efg, "body uart efg");
  body_helper.cm_stg = 0;
  return res;
}