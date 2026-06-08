#include "parameters.h"
#include "fx_api.h"

static parameters_t parameters = {
  .magic = PARAMETERS_MAGIC,
  .pin_sha1 = {0xf3, 0x07, 0x06, 0x3a, 0x85, 0x8b, 0xe3, 0x3f, 0xf2, 0xba, 0x3e, 0xf5, 0xd1, 0xdb, 0x8e, 0x30, 0x6b, 0x62, 0xeb, 0x02},
  .version = PARAMETERS_VERSION
};

HAL_StatusTypeDef parameters_init() {
  FX_FILE file;
  // if (fx_file_open(&file, "parameters.bin", FX_OPEN_FOR_READ) != FX_SUCCESS) {
  //   return HAL_ERROR;
  // }

  // if (fx_file_read(&file, &parameters, sizeof(parameters)) != FX_SUCCESS) {
  //   return HAL_ERROR;
  // }
  // return rk628f_test();
}

parameters_t *parameters_get() {
  return &parameters;
}
