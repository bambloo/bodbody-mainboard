#include "parameters.h"
#include "app_filex.h"
#include "fx_api.h"
#include "fx_file.h"
#include "hash.h"
#include "stm32h7xx_hal_hash.h"
#include "tx_api.h"

static parameters_t parameters = {
    .magic = PARAMETERS_MAGIC,
    .version = PARAMETERS_VERSION,
    .pin_sha1 = {0xf3, 0x07, 0x06, 0x3a, 0x85, 0x8b, 0xe3, 0x3f, 0xf2, 0xba,
                 0x3e, 0xf5, 0xd1, 0xdb, 0x8e, 0x30, 0x6b, 0x62, 0xeb, 0x02},
    .version = PARAMETERS_VERSION};

UINT parameters_init() {
  FX_FILE file;
  UINT status;
  parameters_t params_cache;
  status = fx_file_open(&sdio_disk, &file, "parameters.bin", FX_OPEN_FOR_WRITE);
  if (status != FX_SUCCESS) {
    return FX_ERROR_NOT_FIXED;
  }
  status = fx_read_buffer(&file, &params_cache, sizeof(params_cache));
  if (status != FX_SUCCESS) {
    return FX_ERROR_NOT_FIXED;
  }

  if (params_cache.magic != PARAMETERS_MAGIC ||
      params_cache.version != PARAMETERS_VERSION) {
    return FX_SUCCESS;
  }

  memcpy(&parameters, &params_cache, sizeof(parameters));
  fx_file_close(&file);
  fx_media_flush(&sdio_disk);
  return TX_SUCCESS;

  // if (status == FX_SUCCESS) {

  //   // 3. Write data to the file
  //   status = fx_file_write(&file, "Hello World", 11);

  //   // 4. Close the file to flush the buffer to the SD card
  //   fx_file_close(&file);

  //   // 5. Optional: Flush the media to ensure physical write
  //   fx_media_flush(&sdio_disk);
  // }
  // if (fx_file_open(&file, "parameters.bin", FX_OPEN_FOR_READ) != FX_SUCCESS)
  // {
  //   return HAL_ERROR;
  // }

  // if (fx_file_read(&file, &parameters, sizeof(parameters)) != FX_SUCCESS) {
  //   return HAL_ERROR;
  // }
  // return rk628f_test();
}

UINT parameters_check_pin(uint32_t pin) {
  uint8_t pin_sha1[20];
  HAL_HASH_SHA1_Start(&hhash, (uint8_t *)&pin, sizeof(pin), pin_sha1, 1000);
  return memcmp(pin_sha1, parameters.pin_sha1, sizeof(pin_sha1)) == 0;
}
