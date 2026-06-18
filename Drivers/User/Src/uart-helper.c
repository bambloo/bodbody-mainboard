#include "uart-helper.h"
#include "tx_api.h"

uart_helper_t *helpers[4];
uint8_t        helper_count = 0;

static void uart_helper_handle_read_buffer(uart_helper_t *helper)
{
  int count = __HAL_DMA_GET_COUNTER(helper->uart->hdmarx);
  if (count != helper->rx_lst) {
    if (count <= helper->rx_lst) {
      helper->rx_cnt += (helper->rx_lst - count);
    } else {
      helper->rx_cnt += (helper->rx_lst + sizeof(helper->rx_buf) - count);
    }
    helper->rx_lst = count;
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  for (int i = 0; i < helper_count; i++) {
    if (helpers[i]->uart == huart) {
      HAL_UART_Receive_DMA(helpers[i]->uart, helpers[i]->rx_buf, sizeof(helpers[i]->rx_buf));
      helpers[i]->rx_csr = 0;
      helpers[i]->rx_cnt = 0;
      helpers[i]->rx_lst = sizeof(helpers[i]->rx_buf);
      helpers[i]->tx_csr = 0;
      break;
    }
  }
}

void uart_helper_register(uart_helper_t *helper, UART_HandleTypeDef *uart,
                          uart_data_clbk_t callback, IRQn_Type irqn)
{
  helper->uart      = uart;
  helper->rx_csr    = 0;
  helper->rx_cnt    = 0;
  helper->rx_lst    = sizeof(helper->rx_buf);
  helper->tx_csr    = 0;
  helper->data_clbk = callback;
  helper->irq       = irqn;

  helpers[helper_count++] = helper;

  tx_mutex_create(&helper->mutex, "uart1 mutex", 1);
  HAL_UART_Receive_DMA(uart, helper->rx_buf, sizeof(helper->rx_buf));
}

// static uint32_t read_long(uart_helper_t *helper)
// {
//     return (read_byte(helper) << 24) | (read_byte(helper) << 16) |
//     (read_byte(helper) << 8) | read_byte(helper);
// }

// static int handle_stage(uart_helper_t *helper) {
//   while (helper->rx_cnt) {
//     switch (cmd_stg) {
//     case 0:
//       cmd_tkn = (cmd_tkn << 8) | read_byte(helper);
//       if (cmd_tkn == 0x55AA) {
//         cmd_stg = 1;
//       }
//       break;
//     case 1:
//       if (helper->rx_cnt < 2) {
//         return 0;
//       }
//       cmd_id = read_short(helper);
//       cmd_stg = 2;
//       break;
//     case 2:
//       if (helper->rx_cnt < 2) {
//         return 0;
//       }
//       cmd_key = read_short(helper);
//       cmd_stg = 3;
//       break;
//     case 3:
//       if (helper->rx_cnt < 2) {
//         return 0;
//       }
//       cmd_len = read_short(helper);
//       if (cmd_len > 32) {
//         cmd_stg = 0;
//         return 0;
//       }
//       cmd_stg = 4;
//       break;
//     case 4:
//       if (helper->rx_cnt < cmd_len) {
//         return 0;
//       }

//       for (int i = 0; i < cmd_len; i++) {
//         cmd_buf[i] = read_byte(helper);
//       }
//       cmd_stg = 5;
//       break;
//     case 5:
//       if (helper->rx_cnt < 2) {
//         return 0;
//       }
//       cmd_stg = 0;
//       if (read_short(helper) == 0xAA55) {
//         return 1;
//       } else {
//         return 0;
//       }
//     }
//   }
//   return 0;
// }

static void uart_helper_handle(uart_helper_t *helper)
{
  uart_helper_handle_read_buffer(helper);
  HAL_NVIC_DisableIRQ(helper->irq);
  helper->data_clbk(helper);
  HAL_NVIC_EnableIRQ(helper->irq);
}

void uart_helper_check()
{
  for (int i = 0; i < helper_count; i++) {
    uart_helper_handle(helpers[i]);
  }
}

void uart_helper_send(uart_helper_t *helper, uint16_t id, uint8_t *buf, uint16_t len)
{
  helper->tx_csr                   = 0;
  helper->tx_buf[helper->tx_csr++] = 0x55;
  helper->tx_buf[helper->tx_csr++] = 0xAA;
  helper->tx_buf[helper->tx_csr++] = id >> 8;
  helper->tx_buf[helper->tx_csr++] = id;
  helper->tx_buf[helper->tx_csr++] = len >> 8;
  helper->tx_buf[helper->tx_csr++] = len;
  for (int i = 0; i < len; i++) {
    helper->tx_buf[helper->tx_csr++] = buf[i];
  }
  helper->tx_buf[helper->tx_csr++] = 0xAA;
  helper->tx_buf[helper->tx_csr++] = 0x55;

  _tx_mutex_get(&helper->mutex, TX_WAIT_FOREVER);
  HAL_UART_Transmit(helper->uart, helper->tx_buf, helper->tx_csr, 1000);
  _tx_mutex_put(&helper->mutex);
}
