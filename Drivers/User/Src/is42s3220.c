#include "is42s32200.h"
#include "fmc.h"
#include "rng.h"

#define IS32S32200_DEFAULT_TIMEOUT 1000

HAL_StatusTypeDef is42s32200_init(SDRAM_HandleTypeDef *hsdram) {
  FMC_SDRAM_CommandTypeDef sdram_cmd;
  
  sdram_cmd.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  sdram_cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  sdram_cmd.AutoRefreshNumber = 1;
  sdram_cmd.ModeRegisterDefinition = 0;
  
  HAL_SDRAM_SendCommand(hsdram, &sdram_cmd, IS32S32200_DEFAULT_TIMEOUT);
  
    sdram_cmd.CommandMode = FMC_SDRAM_CMD_PALL;
  sdram_cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  sdram_cmd.AutoRefreshNumber = 1;
  sdram_cmd.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(hsdram, &sdram_cmd, IS32S32200_DEFAULT_TIMEOUT);
  
  sdram_cmd.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  sdram_cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  sdram_cmd.AutoRefreshNumber = 8;
  sdram_cmd.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(hsdram, &sdram_cmd, IS32S32200_DEFAULT_TIMEOUT);
  
  uint32_t mode_register = 0x00000002 | 0x00000000 | 0x00000030 | 0x00000000 | 0x00000200;
  sdram_cmd.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  sdram_cmd.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  sdram_cmd.AutoRefreshNumber = 1;
  sdram_cmd.ModeRegisterDefinition = mode_register;
  HAL_SDRAM_SendCommand(hsdram, &sdram_cmd, IS32S32200_DEFAULT_TIMEOUT);
  
  HAL_SDRAM_ProgramRefreshRate(hsdram, 3750);
  
  return HAL_OK;
}

HAL_StatusTypeDef is42s32200_test() {
  
  unsigned int sdram_bytes = 8 * 1024 * 1024;
  uint32_t seed;
  
  HAL_RNG_GenerateRandomNumber(&hrng, &seed);
  for (unsigned int i = 0xC0000000; i < 0xC0000000 + sdram_bytes; i ++) {
    *(uint8_t*)i = (i + seed);
  }
  for (unsigned int i = 0xC0000000; i < 0xC0000000 + sdram_bytes; i ++) {
    uint8_t val = *(uint8_t*)i;
    if (val != (uint8_t)(i + seed)) {
      val = val + 1;
    }
  }
  
  return HAL_OK;
}