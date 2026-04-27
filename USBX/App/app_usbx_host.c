/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_usbx_host.c
 * @author  MCD Application Team
 * @brief   USBX host applicative file
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
#include "app_usbx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_printer.h"
#include "ux_host_class_printer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static TX_THREAD ux_host_app_thread;
extern HCD_HandleTypeDef hhcd_USB_OTG_HS;

/* USER CODE BEGIN PV */
// UX_HOST_CLASS_HUB *hub_instance = NULL;
// UX_HOST_CLASS_STORAGE *storage_instance = NULL;
// UX_HOST_CLASS_STORAGE_MEDIA *storage_media = NULL;
// FX_MEDIA *fx_media = NULL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_host_thread_entry(ULONG thread_input);
static UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance);
static VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
 * @brief  Application USBX Host Initialization.
 * @param  memory_ptr: memory pointer
 * @retval status
 */
UINT MX_USBX_Host_Init(VOID *memory_ptr) {
  UINT ret = UX_SUCCESS;
  UCHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

  /* USER CODE BEGIN MX_USBX_Host_Init0 */

  /* USER CODE END MX_USBX_Host_Init0 */

  /* Allocate the stack for host application main thread */
  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, UX_HOST_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
    /* USER CODE BEGIN MAIN_THREAD_ALLOCATE_STACK_ERROR */
    return TX_POOL_ERROR;
    /* USER CODE END MAIN_THREAD_ALLOCATE_STACK_ERROR */
  }

  /* Create the host application main thread */
  if (tx_thread_create(&ux_host_app_thread, UX_HOST_APP_THREAD_NAME, app_ux_host_thread_entry, 0, pointer,
                       UX_HOST_APP_THREAD_STACK_SIZE, UX_HOST_APP_THREAD_PRIO, UX_HOST_APP_THREAD_PREEMPTION_THRESHOLD,
                       UX_HOST_APP_THREAD_TIME_SLICE, UX_HOST_APP_THREAD_START_OPTION) != TX_SUCCESS) {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERROR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERROR */
  }

  /* USER CODE BEGIN MX_USBX_Host_Init1 */

  /* USER CODE END MX_USBX_Host_Init1 */

  return ret;
}

/**
 * @brief MX_USBX_Host_Stack_Init
 *        Initialization of USB host stack.
 *        Init USB Host stack, add register the host class stack
 * @param  None
 * @retval ret
 */
UINT MX_USBX_Host_Stack_Init(void) {
  UINT ret = UX_SUCCESS;

  /* USER CODE BEGIN MX_USBX_Host_Stack_Init_PreTreatment */
  /* USER CODE END MX_USBX_Host_Stack_Init_PreTreatment */

  /* Install the host portion of USBX */
  if (ux_host_stack_initialize(ux_host_event_callback) != UX_SUCCESS) {
    /* USER CODE BEGIN USBX_HOST_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_INITIALIZE_ERROR */
  }

  /* Register a callback error function */
  ux_utility_error_callback_register(&ux_host_error_callback);

  /* Initialize the host storage class */
  if (ux_host_stack_class_register(_ux_system_host_class_storage_name, ux_host_class_storage_entry) != UX_SUCCESS) {
    /* USER CODE BEGIN USBX_HOST_STORAGE_REGISTER_ERROR */
    //   return UX_ERROR;
    /* USER CODE END USBX_HOST_STORAGE_REGISTER_ERROR */
  }

  /* Initialize the host hub */
  if (ux_host_stack_class_register(_ux_system_host_class_hub_name, ux_host_class_hub_entry) != UX_SUCCESS) {
    /* USER CODE BEGIN USBX_HOST_HUB_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_HUB_REGISTER_ERROR */
  }

  /* Initialize the host controller driver */
  ux_host_stack_hcd_register(_ux_system_host_hcd_stm32_name, _ux_hcd_stm32_initialize, USB_OTG_HS_PERIPH_BASE,
                             (ULONG)&hhcd_USB_OTG_HS);

  /* USER CODE BEGIN MX_USBX_Host_Stack_Init_PostTreatment */

  ux_host_stack_class_register(_ux_system_host_class_printer_name, _ux_host_class_printer_entry);
  /* USER CODE END MX_USBX_Host_Stack_Init_PostTreatment */

  return ret;
}

/**
 * @brief  MX_USBX_Host_Stack_DeInit
 *         Uninitialize of USB Host stack.
 *         Uninitialize the host stack, unregister of host class stack and
 *         unregister of the usb host controllers
 * @param  None
 * @retval ret
 */
UINT MX_USBX_Host_Stack_DeInit(void) {
  UINT ret = UX_SUCCESS;

  /* USER CODE BEGIN MX_USBX_Host_Stack_DeInit_PreTreatment */

  /* USER CODE END MX_USBX_Host_Stack_DeInit_PreTreatment */

  /* Unregister all the USB host controllers available in this system. */
  ux_host_stack_hcd_unregister(_ux_system_host_hcd_stm32_name, USB_OTG_HS_PERIPH_BASE, (ULONG)&hhcd_USB_OTG_HS);

  /* Unregister the host hub class */
  if (ux_host_stack_class_unregister(ux_host_class_hub_entry) != UX_SUCCESS) {
    return UX_ERROR;
  }

  /* Unregister the host storage class */
  if (ux_host_stack_class_unregister(ux_host_class_storage_entry) != UX_SUCCESS) {
    return UX_ERROR;
  }

  /* The code below is required for uninstalling the host portion of USBX.  */
  if (ux_host_stack_uninitialize() != UX_SUCCESS) {
    return UX_ERROR;
  }

  /* USER CODE BEGIN MX_USBX_Host_Stack_DeInit_PostTreatment */
  ux_host_stack_class_unregister(_ux_host_class_printer_entry);
  /* USER CODE END MX_USBX_Host_Stack_DeInit_PostTreatment */

  return ret;
}

/**
 * @brief  Function implementing app_ux_host_thread_entry.
 * @param  thread_input: User thread input parameter.
 * @retval none
 */
static VOID app_ux_host_thread_entry(ULONG thread_input) {
  /* USER CODE BEGIN app_ux_host_thread_entry */
  TX_PARAMETER_NOT_USED(thread_input);
  MX_USBX_Host_Stack_Init();
  HAL_HCD_Start(&hhcd_USB_OTG_HS);

  /* USER CODE END app_ux_host_thread_entry */
}

/**
 * @brief  ux_host_event_callback
 *         This callback is invoked to notify application of instance changes.
 * @param  event: event code.
 * @param  current_class: Pointer to class.
 * @param  current_instance: Pointer to class instance.
 * @retval status
 */
UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance) {
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN ux_host_event_callback0 */
  UX_PARAMETER_NOT_USED(current_class);
  UX_PARAMETER_NOT_USED(current_instance);
  /* USER CODE END ux_host_event_callback0 */

  switch (event) {
  case UX_DEVICE_INSERTION:

    /* USER CODE BEGIN UX_DEVICE_INSERTION */
    if (current_class->ux_host_class_entry_function == ux_host_class_printer_entry) {
      UX_HOST_CLASS_PRINTER *printer = (UX_HOST_CLASS_PRINTER *)current_instance;
      app_printer_plugin(printer);
    }
    // if (current_class->ux_host_class_entry_function ==
    //     ux_host_class_hub_entry) {
    //   if (hub_instance == NULL) {
    //     hub_instance = current_instance;
    //   }
    // }
    // if (current_class->ux_host_class_entry_function ==
    //     ux_host_class_storage_entry) {

    //   if (storage_instance == NULL) {
    //     storage_instance = current_instance;
    //   } else {
    //     break;
    //   }

    //   /* Get the storage media */
    //   storage_media =
    //       (UX_HOST_CLASS_STORAGE_MEDIA *)current_class->ux_host_class_media;

    //   if (storage_media->ux_host_class_storage_media_lun != 0) {
    //     storage_media = UX_NULL;
    //   }
    // }
    /* USER CODE END UX_DEVICE_INSERTION */

    break;

  case UX_DEVICE_REMOVAL:

    /* USER CODE BEGIN UX_DEVICE_REMOVAL */
    if (current_class->ux_host_class_entry_function == ux_host_class_printer_entry) {
    }
    // if (current_class->ux_host_class_entry_function ==
    //     ux_host_class_hub_entry) {
    //   if (hub_instance == current_instance) {
    //     hub_instance = NULL;
    //   }
    // }

    /* USER CODE END UX_DEVICE_REMOVAL */

    break;

#if defined(UX_HOST_CLASS_STORAGE_NO_FILEX)
  case UX_STORAGE_MEDIA_INSERTION:

    /* USER CODE BEGIN UX_STORAGE_MEDIA_INSERTION */

    /* USER CODE END UX_STORAGE_MEDIA_INSERTION */

    break;

  case UX_STORAGE_MEDIA_REMOVAL:

    /* USER CODE BEGIN UX_STORAGE_MEDIA_REMOVAL */

    /* USER CODE END UX_STORAGE_MEDIA_REMOVAL */

    break;
#endif

  case UX_DEVICE_CONNECTION:

    /* USER CODE BEGIN UX_DEVICE_CONNECTION */

    /* USER CODE END UX_DEVICE_CONNECTION */

    break;

  case UX_DEVICE_DISCONNECTION:

    /* USER CODE BEGIN UX_DEVICE_DISCONNECTION */

    /* USER CODE END UX_DEVICE_DISCONNECTION */

    break;

  default:

    /* USER CODE BEGIN EVENT_DEFAULT */

    /* USER CODE END EVENT_DEFAULT */

    break;
  }

  /* USER CODE BEGIN ux_host_event_callback1 */

  /* USER CODE END ux_host_event_callback1 */

  return status;
}

/**
 * @brief ux_host_error_callback
 *         This callback is invoked to notify application of error changes.
 * @param  system_level: system level parameter.
 * @param  system_context: system context code.
 * @param  error_code: error event code.
 * @retval Status
 */
VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code) {
  /* USER CODE BEGIN ux_host_error_callback0 */
  UX_PARAMETER_NOT_USED(system_level);
  UX_PARAMETER_NOT_USED(system_context);
  /* USER CODE END ux_host_error_callback0 */

  switch (error_code) {
  case UX_DEVICE_ENUMERATION_FAILURE:

    /* USER CODE BEGIN UX_DEVICE_ENUMERATION_FAILURE */

    /* USER CODE END UX_DEVICE_ENUMERATION_FAILURE */

    break;

  case UX_NO_DEVICE_CONNECTED:

    /* USER CODE BEGIN UX_NO_DEVICE_CONNECTED */

    /* USER CODE END UX_NO_DEVICE_CONNECTED */

    break;

  default:

    /* USER CODE BEGIN ERROR_DEFAULT */

    /* USER CODE END ERROR_DEFAULT */

    break;
  }

  /* USER CODE BEGIN ux_host_error_callback1 */

  /* USER CODE END ux_host_error_callback1 */
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
