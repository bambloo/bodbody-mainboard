#include "w25q128.h"
#include "quadspi.h"

#define W25_STATUS_BUSY 0x01

typedef enum {
  W25_INSTRUCTION_LOAD_SR1 = 0x05,
  W25_INSTRUCTION_LOAD_SR2 = 0x35,
  W25_INSTRUCTION_LOAD_SR3 = 0x15,

  W25_INSTRUCTION_WRITE_ENABLE = 0x06,

  W25_INSTRUCTION_ERASE_SECTOR = 0x20,

  W25_INSTRUCTION_QUAD_INPUT_PROGRAM = 0x32,

  W25_INSTRUCTION_READ_UNIQUE_ID = 0x4B,
  W25_INSTRUCTION_READ_JEDEC_ID  = 0x9F,

  W25_INSTRUCTION_READ_DATA      = 0x03,
  W25_INSTRUCTION_READ_DATA_FAST = 0x0B,
  W25_INSTRUCTION_READ_DATA_DUAL = 0x3B,
  W25_INSTRUCTION_READ_DATA_QUAD = 0x6B,

} W25_INSTRUCTION_SET;

HAL_StatusTypeDef w25_load_sr(uint8_t index, uint8_t *val)
{
  QSPI_CommandTypeDef command = {
      .InstructionMode   = QSPI_INSTRUCTION_1_LINE,
      .AddressMode       = QSPI_ADDRESS_NONE,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_1_LINE,

      .DdrMode  = QSPI_DDR_MODE_DISABLE,
      .SIOOMode = QSPI_SIOO_INST_EVERY_CMD,

      .Instruction = index == 2   ? W25_INSTRUCTION_LOAD_SR2
                     : index == 3 ? W25_INSTRUCTION_LOAD_SR3
                                  : W25_INSTRUCTION_LOAD_SR1,
      .DummyCycles = 0,
      .NbData      = 1,
  };

  HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK) {
    return status;
  }

  return HAL_QSPI_Receive(&hqspi, val, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef w25_read_unique_id(uint64_t *id)
{
  QSPI_CommandTypeDef command = {
      .InstructionMode   = QSPI_INSTRUCTION_1_LINE,
      .AddressMode       = QSPI_ADDRESS_NONE,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_1_LINE,

      .DdrMode  = QSPI_DDR_MODE_DISABLE,
      .SIOOMode = QSPI_SIOO_INST_EVERY_CMD,

      .Instruction = W25_INSTRUCTION_READ_UNIQUE_ID,
      .DummyCycles = 32,
      .NbData      = 8,
  };
  HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK) {
    return status;
  }
  return HAL_QSPI_Receive(&hqspi, (uint8_t *)id, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
};

HAL_StatusTypeDef w25_read_jedec_id(uint32_t *id)
{
  *id                         = 0;
  QSPI_CommandTypeDef command = {
      .InstructionMode   = QSPI_INSTRUCTION_1_LINE,
      .AddressMode       = QSPI_ADDRESS_NONE,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_1_LINE,

      .DdrMode  = QSPI_DDR_MODE_DISABLE,
      .SIOOMode = QSPI_SIOO_INST_EVERY_CMD,

      .Instruction = W25_INSTRUCTION_READ_JEDEC_ID,
      .DummyCycles = 0,
      .NbData      = 3,
  };
  HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK) {
    return status;
  }
  return HAL_QSPI_Receive(&hqspi, (uint8_t *)id, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
};

HAL_StatusTypeDef w25_write_enable()
{
  QSPI_CommandTypeDef command = {
      .InstructionMode = QSPI_INSTRUCTION_1_LINE,
      .Instruction     = W25_INSTRUCTION_WRITE_ENABLE,
      .AddressMode     = QSPI_ADDRESS_NONE,

      .DummyCycles       = 0,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_NONE,
      .DdrMode           = QSPI_DDR_MODE_DISABLE,
      .SIOOMode          = QSPI_SIOO_INST_EVERY_CMD,
  };
  return HAL_QSPI_Command(&hqspi, &command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef w25_wait_for_idle()
{
  QSPI_CommandTypeDef command = {
      .InstructionMode   = QSPI_INSTRUCTION_1_LINE,
      .AddressMode       = QSPI_ADDRESS_NONE,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_1_LINE,
      .SIOOMode          = QSPI_SIOO_INST_EVERY_CMD,
      .DdrMode           = QSPI_DDR_MODE_DISABLE,

      .Instruction = W25_INSTRUCTION_LOAD_SR1,
      .DummyCycles = 0,
  };

  QSPI_AutoPollingTypeDef polling = {
      .AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE,
      .Interval        = 0x80,
      .Mask            = 0x01,
      .Match           = 0x00,
      .MatchMode       = QSPI_MATCH_MODE_AND,
      .StatusBytesSize = 0x01
  };

  return HAL_QSPI_AutoPolling(&hqspi, &command, &polling, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef w25_erase_sector(uint32_t sectorAddress)
{
  HAL_StatusTypeDef status = w25_write_enable();

  if (status != HAL_OK) {
    return status;
  }

  HAL_Delay(1);

  QSPI_CommandTypeDef command = {
      .InstructionMode   = QSPI_INSTRUCTION_1_LINE,
      .AddressMode       = QSPI_ADDRESS_1_LINE,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_1_LINE,
      .DdrMode           = QSPI_DDR_MODE_DISABLE,
      .SIOOMode          = QSPI_SIOO_INST_EVERY_CMD,
      .Instruction       = W25_INSTRUCTION_ERASE_SECTOR,
      .AddressSize       = QSPI_ADDRESS_24_BITS,
      .Address           = sectorAddress,
      .DummyCycles       = 0,
      .NbData            = 8,
  };
  status = HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK) {
    return status;
  }
  return w25_wait_for_idle();
}

HAL_StatusTypeDef w25_write(uint32_t addr, uint8_t *buf, int len)
{
  if (len > 256) {
    len = 256;
  }
  HAL_StatusTypeDef status = w25_write_enable();
  if (status != HAL_OK) {
    return status;
  }
  status = w25_erase_sector(addr);
  if (status != HAL_OK) {
    return status;
  }
  status = w25_write_enable();
  if (status != HAL_OK) {
    return status;
  }

  QSPI_CommandTypeDef command = {
      .InstructionMode   = QSPI_INSTRUCTION_1_LINE,
      .AddressMode       = QSPI_ADDRESS_1_LINE,
      .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
      .DataMode          = QSPI_DATA_4_LINES,
      .DdrMode           = QSPI_DDR_MODE_DISABLE,
      .SIOOMode          = QSPI_SIOO_INST_EVERY_CMD,
      .Instruction       = 0x32,
      .AddressSize       = QSPI_ADDRESS_24_BITS,
      .Address           = addr,
      .DummyCycles       = 0,
      .NbData            = len
  };

  status = HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

  status = HAL_QSPI_Transmit(&hqspi, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK) {
    return status;
  }

  return w25_wait_for_idle();
}
HAL_StatusTypeDef w25_read(uint32_t addr, uint8_t *buf, int len)
{
  QSPI_CommandTypeDef command = {
      .InstructionMode    = QSPI_INSTRUCTION_1_LINE,
      .AddressMode        = QSPI_ADDRESS_4_LINES,
      .AlternateByteMode  = QSPI_ALTERNATE_BYTES_4_LINES,
      .DataMode           = QSPI_DATA_4_LINES,
      .DdrMode            = QSPI_DDR_MODE_DISABLE,
      .SIOOMode           = QSPI_SIOO_INST_EVERY_CMD,
      .Instruction        = 0xEB,
      .AddressSize        = QSPI_ADDRESS_24_BITS,
      .Address            = addr,
      .AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS,
      .AlternateBytes     = 0xFF,
      .DummyCycles        = 4,
      .NbData             = len
  };

  HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK) {
    return status;
  }

  return HAL_QSPI_Receive(&hqspi, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef w25_map()
{
  QSPI_CommandTypeDef sCommand = {
      .InstructionMode    = QSPI_INSTRUCTION_1_LINE,
      .AddressMode        = QSPI_ADDRESS_4_LINES,
      .AlternateByteMode  = QSPI_ALTERNATE_BYTES_4_LINES,
      .DataMode           = QSPI_DATA_4_LINES,
      .DdrMode            = QSPI_DDR_MODE_DISABLE,
      .SIOOMode           = QSPI_SIOO_INST_EVERY_CMD,
      .Instruction        = 0xEB,
      .AddressSize        = QSPI_ADDRESS_24_BITS,
      .AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS,
      .AlternateBytes     = 0xFF,
      .DummyCycles        = 4,
  };
  ;
  QSPI_MemoryMappedTypeDef sMemMappedCfg;

  sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeOutPeriod     = 0;

  /* Send the configuration to the QSPI peripheral */
  return HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg);
}
HAL_StatusTypeDef w25_unmap()
{
  return HAL_QSPI_DeInit(&hqspi) || HAL_QSPI_Init(&hqspi);
}
