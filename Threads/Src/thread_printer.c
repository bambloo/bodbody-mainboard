#include "thread_printer.h"
#include "main.h"
#include "ux_api.h"
#include "ux_host_class_printer.h"
#include "hp-1008a.h"

static TX_THREAD printer_thread;
static TX_MUTEX printer_mutex;
static TX_SEMAPHORE printer_sema;
static UX_HOST_CLASS_PRINTER *printer_global;

static void app_printer_entry(ULONG thread_input);

uint8_t app_printer_create(void) {
  UINT ret = TX_SUCCESS;

  tx_mutex_create(&printer_mutex, "PRINTER_MUTEX", 1);
  tx_semaphore_create(&printer_sema, "PRINTER_SEMA", 0);

  void *stack = malloc(DEFAULT_APP_STACK_SIZE * 2);
  if (!stack) {
    return TX_NO_MEMORY;
  }
  ret = tx_thread_create(&printer_thread, "PRINTER", app_printer_entry, 0, stack, DEFAULT_APP_STACK_SIZE, 10, 10,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  return ret;
}
void app_printer_plugin(void *handle) {
  tx_mutex_get(&printer_mutex, TX_WAIT_FOREVER);
  printer_global = (UX_HOST_CLASS_PRINTER *)handle;
  tx_mutex_put(&printer_mutex);
  tx_semaphore_put(&printer_sema);
}

void app_printer_unplugin() {
  tx_mutex_get(&printer_mutex, TX_WAIT_FOREVER);
  printer_global = NULL;
  tx_mutex_put(&printer_mutex);
}

void app_printer_entry(ULONG thread_input) {
  uint8_t *data_buffer = malloc(0x500);
  while (1) {
    if (tx_semaphore_get(&printer_sema, TX_WAIT_FOREVER) != TX_SUCCESS || !printer_global) {
      continue;
    }

    UX_TRANSFER *transfer_request;
    UINT status;

    // status = ux_host_stack_interface_setting_select(
    //     printer->ux_host_class_printer_interface->ux_interface_next_interface);

    transfer_request = &printer_global->ux_host_class_printer_device->ux_device_control_endpoint.ux_endpoint_transfer_request;

    transfer_request->ux_transfer_request_type = 0xa1;
    transfer_request->ux_transfer_request_function = 0x00;
    transfer_request->ux_transfer_request_index = 0x00;
    transfer_request->ux_transfer_request_value = 0x00;
    transfer_request->ux_transfer_request_requested_length = 0x03f1;
    transfer_request->ux_transfer_request_data_pointer = data_buffer;
    status = ux_host_stack_transfer_request(transfer_request);
    if (status) {
      continue;
    }

    transfer_request->ux_transfer_request_type = 0x00;
    transfer_request->ux_transfer_request_function = 0x0b;
    transfer_request->ux_transfer_request_index = 0x00;
    transfer_request->ux_transfer_request_value = 0x00;
    transfer_request->ux_transfer_request_requested_length = 0x00;
    transfer_request->ux_transfer_request_data_pointer = data_buffer;
    status = ux_host_stack_transfer_request(transfer_request);
    if (status) {
      continue;
    }

    transfer_request->ux_transfer_request_type = 0xC1;
    transfer_request->ux_transfer_request_function = 0x23;
    transfer_request->ux_transfer_request_value = 0x0100;
    transfer_request->ux_transfer_request_index = 0x0000;
    transfer_request->ux_transfer_request_data_pointer = data_buffer;
    transfer_request->ux_transfer_request_requested_length = 8;
    status = ux_host_stack_transfer_request(transfer_request);
    if (status) {
      continue;
    }

    status = ux_host_stack_transfer_request(transfer_request);
    if (status) {
      continue;
    }

    transfer_request->ux_transfer_request_function = 0xe0;
    transfer_request->ux_transfer_request_requested_length = 0xFF;
    status = ux_host_stack_transfer_request(transfer_request);
    if (status) {
      continue;
    }

    transfer_request->ux_transfer_request_function = 0x43;
    transfer_request->ux_transfer_request_value = 0x0000;
    transfer_request->ux_transfer_request_requested_length = 256;
    status = ux_host_stack_transfer_request(transfer_request);
    if (status) {
      continue;
    }

    hp_print();

    // bytes_remain = cursor - send_buf;
    // status = ux_host_class_printer_write(printer, send_buf, bytes_remain, &bytes_written);

    // if (status == TX_SUCCESS) {
    //   bytes_remain -= bytes_written;
    //   cursor += bytes_written;
    // } else {
    //   break;
    // }
  }
}