
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_filex.c
 * @author  MCD Application Team
 * @brief   FileX applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Includes ------------------------------------------------------------------*/
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fx_media.h"
#include "main.h"
#include "parameters.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* Main thread stack size */
#define FX_APP_THREAD_STACK_SIZE         (DEFAULT_APP_STACK_SIZE  * 2)
/* Main thread priority */
#define FX_APP_THREAD_PRIO               10

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#if 0
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Main thread global data structures.  */
TX_THREAD       fx_app_thread;

/* Buffer for FileX FX_MEDIA sector cache. */
ALIGN_32BYTES (uint32_t fx_sd_media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);
/* Define FileX global data structures.  */
FX_MEDIA        sdio_disk;

/* USER CODE BEGIN PV */
#endif
#undef fx_sd_media_memory
uint32_t *fx_sd_media_memory;
TX_THREAD fx_app_thread;
FX_MEDIA sdio_disk;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* Main thread entry function.  */
void filex_thread_entry(ULONG thread_input);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application FileX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_FileX_Init(VOID *memory_ptr)
{
  UINT ret = FX_SUCCESS;

  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  VOID *pointer;

  /* USER CODE BEGIN MX_FileX_MEM_POOL */

  /* USER CODE END MX_FileX_MEM_POOL */

  /* USER CODE BEGIN 0 */

  /* USER CODE END 0 */

  /*Allocate memory for the main thread's stack*/
  ret = tx_byte_allocate(byte_pool, &pointer, FX_APP_THREAD_STACK_SIZE, TX_NO_WAIT);

  /* Check FX_APP_THREAD_STACK_SIZE allocation*/
  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread.  */
  ret = tx_thread_create(&fx_app_thread, FX_APP_THREAD_NAME, filex_thread_entry, 0, pointer, FX_APP_THREAD_STACK_SIZE,
                         FX_APP_THREAD_PRIO, FX_APP_PREEMPTION_THRESHOLD, FX_APP_THREAD_TIME_SLICE, FX_APP_THREAD_AUTO_START);

  /* Check main thread creation */
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  /* USER CODE BEGIN MX_FileX_Init */
  fx_sd_media_memory = (uint32_t *)malloc(FX_STM32_SD_DEFAULT_SECTOR_SIZE);
  /* USER CODE END MX_FileX_Init */

  /* Initialize FileX.  */
  fx_system_initialize();

  /* USER CODE BEGIN MX_FileX_Init 1*/

  /* USER CODE END MX_FileX_Init 1*/

  return ret;
}

 /**
 * @brief  Main thread entry.
 * @param thread_input: ULONG user argument used by the thread entry
 * @retval none
 */
void filex_thread_entry(ULONG thread_input)
{
  UINT sd_status = FX_SUCCESS;
  /* USER CODE BEGIN filex_thread_entry 0 */
#if 0
  /* USER CODE END filex_thread_entry 0 */

  /* Open the SD disk driver */
  sd_status =  fx_media_open(&sdio_disk, FX_SD_VOLUME_NAME, fx_stm32_sd_driver, (VOID *)FX_NULL, (VOID *) fx_sd_media_memory, sizeof(fx_sd_media_memory));

  /* Check the media open sd_status */
  if (sd_status != FX_SUCCESS)
  {
    /* USER CODE BEGIN SD open error */
    if (sd_status == FX_BUFFER_ERROR) {
    }
    /* USER CODE END SD open error */
  }

  /* USER CODE BEGIN filex_thread_entry 1 */
#endif
  sd_status = fx_media_open(&sdio_disk, FX_SD_VOLUME_NAME, fx_stm32_sd_driver,
                            (VOID *)FX_NULL, (VOID *)fx_sd_media_memory,
                            FX_STM32_SD_DEFAULT_SECTOR_SIZE);
  if (sd_status != FX_SUCCESS) {
    return;
  }
  UINT status = fx_file_create(&sdio_disk, "parameters.bin");
  if (status != FX_SUCCESS && status != FX_ALREADY_CREATED) {
    // Handle error (e.g., SD card not ready or media full)
  }

  parameters_init();
  /* USER CODE END filex_thread_entry 1 */
}

/* USER CODE BEGIN 2 */

UINT fx_read_buffer(FX_FILE *file, void *buffer, UINT size) {
  UINT cursor = 0;
  while (cursor < size) {
    ULONG bytes_read = 0;
    UINT status =
        fx_file_read(file, buffer + cursor, size - cursor, &bytes_read);
    if (status != FX_SUCCESS) {
      return status;
    }
    cursor += bytes_read;
  }
  return FX_SUCCESS;
}
/* USER CODE END 2 */
