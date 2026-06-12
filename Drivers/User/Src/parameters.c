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
};

UINT parameters_init() {
  FX_FILE file;
  UINT status;
  parameters_t params_cache;
  status = fx_file_open(&sdio_disk, &file, "parameters.bin", FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS) {
    return FX_ERROR_NOT_FIXED;
  }
  status = fx_read_buffer(&file, &params_cache, sizeof(params_cache));
  if (status != FX_SUCCESS) {
    goto end;
  }

  if (params_cache.magic != PARAMETERS_MAGIC ||
      params_cache.version != PARAMETERS_VERSION) {
    return FX_SUCCESS;
  }

  memcpy(&parameters, &params_cache, sizeof(parameters));
end:
  fx_file_close(&file);
  fx_media_flush(&sdio_disk);
  return status;
}

UINT parameters_check_pin(uint32_t pin) {
  uint8_t pin_sha1[20];
  HAL_HASH_SHA1_Start(&hhash, (uint8_t *)&pin, sizeof(pin), pin_sha1, 1000);
  return memcmp(pin_sha1, parameters.pin_sha1, sizeof(pin_sha1)) == 0;
}

UINT parameters_save_pin(uint32_t pin) {
  HAL_HASH_SHA1_Start(&hhash, (uint8_t *)&pin, sizeof(pin), parameters.pin_sha1,
                      1000);

  FX_FILE file;
  UINT status = fx_file_create(&sdio_disk, "parameters.bin");
  status = fx_file_open(&sdio_disk, &file, "parameters.bin", FX_OPEN_FOR_WRITE);
  if (status != FX_SUCCESS) {
    return status;
  }
  status = fx_file_write(&file, &parameters, sizeof(parameters));
  status = fx_file_close(&file);
  status = fx_media_flush(&sdio_disk);

  return status;
}
