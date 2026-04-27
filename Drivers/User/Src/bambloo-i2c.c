#include "bambloo-i2c.h"
#include "i2c.h"
#include "main.h"
#include "tx_api.h"

static TX_MUTEX sw_i2c_mutex;

#if USE_SOFTWARE_I2C
// --- Configuration: Change these to match your hardware ---
#define SW_I2C_PORT GPIOB
#define SW_I2C_SCL_PIN GPIO_PIN_8
#define SW_I2C_SDA_PIN GPIO_PIN_7
// ----------------------------------------------------------

#define SW_I2C_SPIN_TIMEOUT 10000
// Helper Macros for Speed
#define SCL_H()                                                                \
  do {                                                                         \
    HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN, GPIO_PIN_SET);              \
    sw_i2c_delay_h();                                                          \
  } while (0)
#define SCL_L()                                                                \
  do {                                                                         \
    HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN, GPIO_PIN_RESET);            \
    sw_i2c_delay();                                                            \
  } while (0)
#define SDA_H()                                                                \
  do {                                                                         \
    HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN, GPIO_PIN_SET);              \
    sw_i2c_delay_h();                                                          \
  } while (0)
#define SDA_L()                                                                \
  do {                                                                         \
    HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN, GPIO_PIN_RESET);            \
    sw_i2c_delay();                                                            \
  } while (0)
#define SDA_R() HAL_GPIO_ReadPin(SW_I2C_PORT, SW_I2C_SDA_PIN)
#define SCL_R() HAL_GPIO_ReadPin(SW_I2C_PORT, SW_I2C_SCL_PIN)
#define SCL_HC()                                                               \
  do {                                                                         \
    if (scl_hc()) {                                                            \
      return HAL_ERROR;                                                        \
    }                                                                          \
  } while (0)

#define SDA_HC()                                                               \
  do {                                                                         \
    if (sda_hc()) {                                                            \
      return HAL_ERROR;                                                        \
    }                                                                          \
  } while (0)

static uint8_t sw_i2c_delay(void) {
  volatile int i;
  for (i = 0; i < 200; i++)
    __NOP(); // Adjust this value for 100kHz/400kHz
  return HAL_OK;
}

static uint8_t sw_i2c_delay_h(void) {
  volatile int i;
  for (i = 0; i < 200; i++)
    __NOP(); // Adjust this value for 100kHz/400kHz

  return HAL_OK;
}
static inline void scl_l() {
  HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN, GPIO_PIN_RESET);
  sw_i2c_delay();
}

static inline void sda_l() {
  HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN, GPIO_PIN_RESET);
  sw_i2c_delay();
}

static inline uint8_t sda_r() {
  return HAL_GPIO_ReadPin(SW_I2C_PORT, SW_I2C_SDA_PIN);
}

static inline uint8_t scl_r() {
  return HAL_GPIO_ReadPin(SW_I2C_PORT, SW_I2C_SCL_PIN);
}

static inline void scl_h() {
  HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN, GPIO_PIN_SET);
  sw_i2c_delay_h();
}

static inline uint8_t scl_hc() {
  HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN, GPIO_PIN_SET);
  int count = 0;
  while (!scl_r()) {
    if (++count >= SW_I2C_SPIN_TIMEOUT) {
      return HAL_ERROR;
    }
  }
  return sw_i2c_delay_h();
}

static inline void sda_h() {
  HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN, GPIO_PIN_SET);
  sw_i2c_delay_h();
}

static inline uint8_t sda_hc() {
  HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN, GPIO_PIN_SET);
  int count = 0;
  while (!SDA_R()) {
    if (++count >= SW_I2C_SPIN_TIMEOUT) {
      return HAL_ERROR;
    }
  }
  return sw_i2c_delay_h();
}

static uint8_t sw_i2c_wait_ack() {
  int count = 0;
  scl_l();
  sda_h();
  scl_hc();

  while (sda_r()) {
    if (++count >= 10000) {
      scl_h();
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

/**
 * @brief Generate I2C Start Condition
 */
static uint8_t sw_i2c_start(void) {
  scl_h();
  if (!scl_r() || !sda_r()) {
    SW_I2C_PORT->OTYPER &= ~(SW_I2C_SCL_PIN | SW_I2C_SDA_PIN);
    scl_l();
    sda_l();
    scl_h();
    sda_h();
    SW_I2C_PORT->OTYPER |= SW_I2C_SCL_PIN | SW_I2C_SDA_PIN;
  }
  if (sda_hc() || scl_hc()) {
    return HAL_ERROR;
  }

  sda_l();
  return HAL_OK;
}

static uint8_t sw_i2c_send(uint8_t data) {
  uint8_t i;

  for (i = 0; i < 8; i++) {
    scl_l(); // occupy the bus
    if (data & 0x80)
      SDA_HC();
    else
      sda_l(); // change sda to low
    data <<= 1;
    SCL_HC();
  }
  return sw_i2c_wait_ack();
}

static uint8_t sw_i2c_send_ack(uint8_t ack) {
  scl_l();
  if (ack) {
    sda_l();
  } else {
    sda_h();
  }
  SCL_HC();
  return HAL_OK;
}

static uint8_t sw_i2c_recv(uint8_t *res, uint8_t ack) {
  uint8_t recv = 0;
  scl_l();
  sda_h();
  scl_hc();
  recv = (recv << 1) | sda_r();
  for (int i = 0; i < 7; i++) {
    scl_l();
    scl_hc();
    recv = (recv << 1) | sda_r();
  }
  *res = recv;
  return sw_i2c_send_ack(ack);
}

/**
 * @brief Generate I2C Stop Condition
 */
static uint8_t sw_i2c_stop(void) {
  scl_l();
  sda_l();
  SCL_HC();
  SDA_HC();
  return HAL_OK;
}
#endif

HAL_StatusTypeDef Bambloo_I2C_Init(void) {
#if USE_SOFTWARE_I2C
  HAL_I2C_DeInit(&hi2c1);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  sw_i2c_stop();
#else
  HAL_I2C_Init(&hi2c1);
#endif
  return tx_mutex_create(&sw_i2c_mutex, "SW I2C Mutex", 1);
}

HAL_StatusTypeDef Bambloo_I2C_Mem_Read(uint16_t dev_addr, uint32_t mem_addr,
                                      uint8_t *data, int size, int addr_size) {
  uint8_t status = HAL_OK;
#if USE_SOFTWARE_I2C
  tx_mutex_get(&sw_i2c_mutex, TX_WAIT_FOREVER);
  // 1. Start
  if (sw_i2c_start() || sw_i2c_send(dev_addr | 0)) {
    status = HAL_ERROR;
    goto end;
  }

  for (int i = (addr_size - 1) * 8; i >= 0; i -= 8) {
    if (sw_i2c_send(mem_addr >> i)) {
      status = HAL_ERROR;
      goto end;
    }
  }

  if (sw_i2c_start() || sw_i2c_send(dev_addr | 1)) {
    status = HAL_ERROR;
    goto end;
  }

  for (int i = 1; i < size; i++) {
    if (sw_i2c_recv(data++, 1)) {
      status = HAL_ERROR;
      goto end;
    }
  }
  status = sw_i2c_recv(data, 0) || sw_i2c_stop();
end:
#else
  status = HAL_I2C_Mem_Read(&hi2c1, dev_addr, mem_addr, I2C_MEMADD_SIZE_16BIT,
                            data, size, 1000);
#endif

  tx_mutex_put(&sw_i2c_mutex);
  return status;
}

HAL_StatusTypeDef Bambloo_I2C_Mem_Write(uint16_t dev_addr, uint32_t mem_addr,
                                        uint8_t *data, int size,
                                        int addr_size) {
  uint8_t status = HAL_OK;
#if USE_SOFTWARE_I2C
  tx_mutex_get(&sw_i2c_mutex, TX_WAIT_FOREVER);
  if (sw_i2c_start() || sw_i2c_send(dev_addr | 0)) {
    status = HAL_ERROR;
    goto end;
  }

  for (int i = (addr_size - 1) * 8; i >= 0; i -= 8) {
    if (sw_i2c_send(mem_addr >> i)) {
      status = HAL_ERROR;
      goto end;
    }
  }

  for (int i = 0; i < size; i++) {
    if (sw_i2c_send(*data++)) {
      status = HAL_ERROR;
      goto end;
    }
  }
  status = sw_i2c_stop();
end:
#else
  status = HAL_I2C_Mem_Write(&hi2c1, dev_addr, mem_addr, I2C_MEMADD_SIZE_16BIT,
                             data, size, 1000);
#endif
  tx_mutex_put(&sw_i2c_mutex);
  return status;
}
