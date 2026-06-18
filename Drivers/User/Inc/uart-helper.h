#ifndef _UART_HELPER_H_
#define _UART_HELPER_H_

#include "tx_api.h"
#include "usart.h"

#define BUF_SIZE 256
struct uart_helper;
typedef void (*uart_data_clbk_t)(struct uart_helper *helper);
typedef struct uart_helper {
  UART_HandleTypeDef *uart;
  uint16_t tx_csr;
  uint16_t rx_csr;
  uint16_t rx_lst;
  uint16_t rx_cnt;
  uint8_t tx_buf[BUF_SIZE];
  uint8_t rx_buf[BUF_SIZE];
  uint8_t cm_buf[BUF_SIZE];

  uint8_t cm_stg;

  IRQn_Type irq;

  uart_data_clbk_t data_clbk;

  TX_MUTEX mutex;
} uart_helper_t;

void uart_helper_register(uart_helper_t *uart_helper, UART_HandleTypeDef *uart,
                          uart_data_clbk_t callback, IRQn_Type irq);
void uart_helper_send(uart_helper_t *helper, uint16_t id, uint8_t *buf,
                      uint16_t len);
void uart_helper_check();

static inline uint8_t uart_helper_read_byte(uart_helper_t *helper) {
  uint8_t byte = helper->rx_buf[helper->rx_csr++];
  helper->rx_csr = helper->rx_csr & (BUF_SIZE - 1);
  helper->rx_cnt -= 1;
  return byte;
}

static inline uint16_t uart_helper_read_short(uart_helper_t *helper) {
  return (uart_helper_read_byte(helper) << 8) | uart_helper_read_byte(helper);
}

#endif