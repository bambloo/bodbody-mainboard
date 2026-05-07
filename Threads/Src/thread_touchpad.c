#include "thread_touchpad.h"
#include "thread_ltdc.h"
#include "dma2d.h"
#include "gpio.h"
#include "ltdc.h"
#include "main.h"
#include "rng.h"
#include "stdlib.h"
#include "bambloo-i2c.h"
#include "tx_api.h"

uint8_t stack[4096];
const uint8_t gt911_address = 0x28;

typedef struct {
  // 0x8047-0x805A: ???????
  uint8_t config_version;     // 0x8047: ?????
  uint8_t x_output_max_low;   // 0x8048: X??????(???)
  uint8_t x_output_max_high;  // 0x8049: X??????(???)
  uint8_t y_output_max_low;   // 0x804A: Y??????(???)
  uint8_t y_output_max_high;  // 0x804B: Y??????(???)
  uint8_t touch_number;       // 0x804C: ???????(1-5)
  uint8_t module_switch1;     // 0x804D: ????1(???/?????)
  uint8_t module_switch2;     // 0x804E: ????2(??/HotKnot???)
  uint8_t shake_count;        // 0x804F: ????/????????
  uint8_t filter;             // 0x8050: ??????
  uint8_t large_touch;        // 0x8051: ???????
  uint8_t noise_reduction;    // 0x8052: ???(0-15)
  uint8_t screen_touch_level; // 0x8053: ??????
  uint8_t screen_leave_level; // 0x8054: ??????
  uint8_t low_power_control;  // 0x8055: ?????????(0-15s)
  uint8_t refresh_rate;       // 0x8056: ??????(5+N ms)
  uint8_t x_threshold;        // 0x8057: X??????
  uint8_t y_threshold;        // 0x8058: Y??????
  uint8_t x_speed_limit;      // 0x8059: X?????(??)
  uint8_t y_speed_limit;      // 0x805A: Y?????(??)

  // 0x805B-0x806A: ???????
  uint8_t space_top_bottom;    // 0x805B: ??/??????(??32)
  uint8_t space_left_right;    // 0x805C: ??/??????(??32)
  uint8_t mini_filter;         // 0x805D: ??????????
  uint8_t stretch_r0;          // 0x805E: ????1
  uint8_t stretch_r1;          // 0x805F: ????2
  uint8_t stretch_r2;          // 0x8060: ????3
  uint8_t stretch_rm;          // 0x8061: ????????
  uint8_t drv_groupa_num;      // 0x8062: ???A??
  uint8_t drv_groupb_num;      // 0x8063: ???B??
  uint8_t sensor_num;          // 0x8064: ????A/B??
  uint8_t freqa_factor;        // 0x8065: ???A????
  uint8_t freqb_factor;        // 0x8066: ???B????
  uint8_t pannel_bitfreql;     // 0x8067: ????(???)
  uint8_t pannel_bitfreqh;     // 0x8068: ????(???)
  uint8_t pannel_sensor_timel; // 0x8069: ????????(???)
  uint8_t pannel_sensor_timeh; // 0x806A: ????????(???)

  // 0x806B-0x807E: ???????
  uint8_t pannel_tx_gain;          // 0x806B
  uint8_t pannel_rx_gain;          // 0x806C
  uint8_t pannel_dump_shift;       // 0x806D
  uint8_t drv_frame_controle;      // 0x806E: ??????(0-7)
  uint8_t channel_level_up;        // 0x806F: PGA????
  uint8_t module_switch3;          // 0x8070: ????3(????/????)
  uint8_t gesture_dis;             // 0x8071: ????????
  uint8_t gesture_long_press_time; // 0x8072: ??????????
  uint8_t x_y_slope_adjust;        // 0x8073: X/Y???????
  uint8_t gesture_control;         // 0x8074: ????????/PGA??
  uint8_t gesture_switch1;         // 0x8075: ????1(????)
  uint8_t gesture_switch2;         // 0x8076: ????2(??/??)
  uint8_t gesture_refresh_rate;    // 0x8077: ????????
  uint8_t gesture_touch_level;     // 0x8078: ????????
  uint8_t newgreenwakeuplevel;     // 0x8079: ?????????
  uint8_t freq_hopping_start;      // 0x807A: ??????
  uint8_t freq_hopping_end;        // 0x807B: ??????
  uint8_t noise_detect_times;      // 0x807C: ??????
  uint8_t hopping_flag;            // 0x807D: ????/??????
  uint8_t hopping_threshold;       // 0x807E

  // 0x807F-0x8091: ???????
  uint8_t noise_threshold;        // 0x807F: ??????
  uint8_t noise_min_threshold;    // 0x8080: ??????
  uint8_t nc_8081;                // 0x8081: ??
  uint8_t hopping_sensor_group;   // 0x8082: ????????
  uint8_t hopping_seg1_normalize; // 0x8083: ??1?????
  uint8_t hopping_seg1_factor;    // 0x8084: ??1?????
  uint8_t main_clock_adjust;      // 0x8085: ?????(-7?+8)
  uint8_t hopping_seg2_normalize; // 0x8086: ??2?????
  uint8_t hopping_seg2_factor;    // 0x8087: ??2?????
  uint8_t nc_8088;                // 0x8088: ??
  uint8_t hopping_seg3_normalize; // 0x8089: ??3?????
  uint8_t hopping_seg3_factor;    // 0x808A: ??3?????
  uint8_t nc_808b;                // 0x808B: ??
  uint8_t hopping_seg4_normalize; // 0x808C: ??4?????
  uint8_t hopping_seg4_factor;    // 0x808D: ??4?????
  uint8_t nc_808e;                // 0x808E: ??
  uint8_t hopping_seg5_normalize; // 0x808F: ??5?????
  uint8_t hopping_seg5_factor;    // 0x8090: ??5?????
  uint8_t nc_8091;                // 0x8091: ??

  // 0x8092-0x80A0: ????
  uint8_t hopping_seg6_normalize; // 0x8092: ??6?????
  uint8_t key1;                   // 0x8093: ??1??
  uint8_t key2;                   // 0x8094: ??2??
  uint8_t key3;                   // 0x8095: ??3??
  uint8_t key4;                   // 0x8096: ??4??
  uint8_t key_area;               // 0x8097: ??????/??????
  uint8_t key_touch_level;        // 0x8098: ??????
  uint8_t key_leave_level;        // 0x8099: ??????
  uint8_t key_sens1;              // 0x809A: ??1-2?????
  uint8_t key_sens2;              // 0x809B: ??3-4?????
  uint8_t key_restrain;           // 0x809C: ??????
  uint8_t key_restrain_time;      // 0x809D: ??????
  uint8_t large_touch_gesture;    // 0x809E: ???????????
  uint8_t nc_809f;                // 0x809F: ??
  uint8_t nc_80a0;                // 0x80A0: ??

  // 0x80A1-0x80B6: HotKnot??
  uint8_t hotknot_noise_map; // 0x80A1: ?????
  uint8_t link_threshold;    // 0x80A2: ????
  uint8_t pxy_threshold;     // 0x80A3: ??????
  uint8_t ghot_dump_shift;   // 0x80A4: ????????
  uint8_t ghot_rx_gain;      // 0x80A5: ??????
  uint8_t freq_gain0;        // 0x80A6: 400K/450K????
  uint8_t freq_gain1;        // 0x80A7: 300K/350K????
  uint8_t freq_gain2;        // 0x80A8: 200K/250K????
  uint8_t freq_gain3;        // 0x80A9: 150K????
  uint8_t gap0[9];           // 0x80AA - 0x80B2

  uint8_t combine_dis; // 0x80B3: ??????????
  uint8_t split_set;   // 0x80B4: ??????
  uint8_t gap1[2];     // 0x80B5 - 0x80B6

  // 0x80B7-0x80D4: ????????
  uint8_t sensor_ch[14]; // 0x80B7 - 0x80C4
  uint8_t gap2[16];      // 0x80C5 - 0x80D4

  uint8_t driver_ch[26]; // 0x80D5 - 0x80EE
  uint8_t gap3[16];      // 0x80EF - 0x80FE

  uint8_t chksum; // 0x80FF: ?????(0x8047-0x80FE)
  uint8_t fresh;  // 0x8100: ??????(????)
} gt911_config_t;

static gt911_config_t gt911_config = {
    .config_version = 0x63,
    .x_output_max_low = 0x58,
    .x_output_max_high = 0x2,
    .y_output_max_low = 0x20,
    .y_output_max_high = 0x03,
    .touch_number = 0x01,
    .module_switch1 = 0x05,
    .module_switch2 = 0x00,
    .shake_count = 0x01,
    .filter = 0x09,
    .large_touch = 0x29,

    .noise_reduction = 0x0F,
    .screen_touch_level = 0x55,
    .screen_leave_level = 0x32,
    .low_power_control = 0x03,
    .refresh_rate = 0x00,
    .x_threshold = 0x00,
    .y_threshold = 0x00,
    .x_speed_limit = 0x00,
    .y_speed_limit = 0x00,
    .space_top_bottom = 0x00,
    .space_left_right = 0x00,

    .mini_filter = 0x00,
    .stretch_r0 = 0x00,
    .stretch_r1 = 0x00,
    .stretch_r2 = 0x00,
    .stretch_rm = 0x00,
    .drv_groupa_num = 0x8C,
    .drv_groupb_num = 0x2E,
    .sensor_num = 0x0E,
    .freqa_factor = 0x29,
    .freqb_factor = 0x27,
    .pannel_bitfreql = 0x00,

    .pannel_bitfreqh = 0x3D,
    .pannel_sensor_timel = 0x00,
    .pannel_sensor_timeh = 0x00,
    .pannel_tx_gain = 0x00,
    .pannel_rx_gain = 0x92,
    .pannel_dump_shift = 0x03,
    .drv_frame_controle = 0x1C,
    .channel_level_up = 0x00,
    .module_switch3 = 0x00,
    .gesture_dis = 0x00,
    .gesture_long_press_time = 0x00,

    .x_y_slope_adjust = 0x00,
    .gesture_control = 0x03,
    .gesture_switch1 = 0x64,
    .gesture_switch2 = 0x32,
    .gesture_refresh_rate = 0x00,
    .gesture_touch_level = 0x00,
    .newgreenwakeuplevel = 0x00,
    .freq_hopping_start = 0x00,
    .freq_hopping_end = 0x00,
    .noise_detect_times = 0x00,
    .hopping_flag = 0x00,

    .hopping_threshold = 0x00,
    .noise_threshold = 0x00,
    .noise_min_threshold = 0x00,
    .nc_8081 = 0x00,
    .hopping_sensor_group = 0x00,
    .hopping_seg1_normalize = 0x00,
    .hopping_seg1_factor = 0x00,
    .main_clock_adjust = 0x00,
    .hopping_seg2_normalize = 0x00,
    .hopping_seg2_factor = 0x00,
    .nc_8088 = 0x00,

    .hopping_seg3_normalize = 0x00,
    .hopping_seg3_factor = 0x00,
    .nc_808b = 0x00,
    .hopping_seg4_normalize = 0x00,
    .hopping_seg4_factor = 0x00,
    .nc_808e = 0x00,
    .hopping_seg5_normalize = 0x00,
    .hopping_seg5_factor = 0x00,
    .nc_8091 = 0x00,
    .hopping_seg6_normalize = 0x00,
    .key1 = 0x00,

    .key2 = 0x00,
    .key3 = 0x00,
    .key4 = 0x00,
    .key_area = 0x00,
    .key_touch_level = 0x00,
    .key_leave_level = 0x00,
    .key_sens1 = 0x00,
    .key_sens2 = 0x00,
    .key_restrain = 0x00,
    .key_restrain_time = 0x00,
    .large_touch_gesture = 0x00,

    .nc_809f = 0x00,
    .nc_80a0 = 0x00,
    .hotknot_noise_map = 0x00,
    .link_threshold = 0x00,
    .pxy_threshold = 0x00,
    .ghot_dump_shift = 0x00,
    .ghot_rx_gain = 0x00,
    .freq_gain0 = 0x00,
    .freq_gain1 = 0x00,
    .freq_gain2 = 0x00,
    .freq_gain3 = 0x00,

    .gap0 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .combine_dis = 0x00,
    .split_set = 0x00,

    .gap1 = {0x00, 0x00},
    .sensor_ch = {0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14,
                  0x16, 0x18, 0x1A, 0x1C},
    .gap2 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    .driver_ch = {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0xF,  0x10,
                  0x12, 0x13, 0x14, 0x16, 0x18, 0x1C, 0x1D, 0x1E, 0x1F,
                  0x20, 0x21, 0x22, 0x24, 0x26, 0x28, 0x29, 0x2A},
    //    .sensor_ch = {0x1A, 0x18, 0x16, 0x14, 0x12, 0x10, 0x0E, 0x0C, 0x0A,
    //    0x08, 0x06, 0x04, 0x02, 0x00}, .gap2 = {0x00, 0x00, 0x00, 0x00, 0x00,
    //    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    //    .driver_ch = {0x29, 0x28, 0x24, 0x22, 0x20, 0x1F, 0x1E, 0x1D, 0x0E,
    //    0x0C, 0x0A, 0x08, 0x06, 0x05, 0x04, 0x02, 0x00, 0xFF, 0x00, 0x00,
    //    0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .gap3 = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    .chksum = 0x00,
    .fresh = 0x01,
};
static TX_THREAD touchpad_thread;
static TX_EVENT_FLAGS_GROUP touchpad_efg;

void gt911_reset() {
  TP_INT_GPIO_Port->PUPDR |= 0x01 << 14;
  HAL_GPIO_WritePin(TP_RESET_GPIO_Port, TP_RESET_Pin, GPIO_PIN_RESET);
  tx_thread_sleep(50);
  HAL_GPIO_WritePin(TP_RESET_GPIO_Port, TP_RESET_Pin, GPIO_PIN_SET);
  tx_thread_sleep(10);
  TP_INT_GPIO_Port->PUPDR &= ~(0x03 << 14);
  tx_thread_sleep(50);
}

void thread_touchpad_entry(ULONG thread_input);

uint8_t thread_touchpad_create(void) {
  UINT ret = TX_SUCCESS;
  //  void *stack = malloc(DEFAULT_APP_STACK_SIZE);
  ret = tx_thread_create(&touchpad_thread, "LTDC", thread_touchpad_entry, 0, stack,
                         DEFAULT_APP_STACK_SIZE, 10, 10, TX_NO_TIME_SLICE,
                         TX_AUTO_START);

  return ret;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  tx_event_flags_set(&touchpad_efg, 0x01, TX_OR);
}

HAL_StatusTypeDef gt911_read_pid() {
  gt911_config_t valid_config;

  HAL_StatusTypeDef status;
  uint8_t info_bytes[0x0B];
  status =
      Bambloo_I2C_Mem_Read(gt911_address, 0x8140, info_bytes, sizeof(info_bytes), 2);
  // status = BamblooI2C_Mem_Read(gt911_address, 0x8047, (uint8_t *)&valid_config,
  //                          sizeof(valid_config));
  tx_thread_sleep(50);
  gt911_config.config_version = valid_config.config_version + 1;
  uint8_t checksum = 0;
  for (int i = 0; i < sizeof(gt911_config) - 2; i++) {
    checksum += ((uint8_t *)&gt911_config)[i];
  }
  checksum = (~checksum);
  gt911_config.chksum = checksum + 1;
  gt911_config.fresh = 1;
  status = Bambloo_I2C_Mem_Write(gt911_address, 0x8047, (uint8_t *)&gt911_config,
                            sizeof(gt911_config), 2);
  tx_thread_sleep(20);

  status = Bambloo_I2C_Mem_Read(gt911_address, 0x8047, (uint8_t *)&valid_config,
                           sizeof(valid_config), 2);
  tx_thread_sleep(20);
  return status;
}

void thread_touchpad_entry(ULONG thread_input) {
  uint32_t flags;
  uint32_t count = 0;
  uint8_t status_register, temp_register = 0, coors[4];

reinit:
  gt911_reset();
  gt911_read_pid();

  while (1) {
    tx_event_flags_get(&touchpad_efg, 0x01, TX_AND_CLEAR, &flags,
                       TX_WAIT_FOREVER);

    HAL_StatusTypeDef status =
        Bambloo_I2C_Mem_Read(gt911_address, 0x814E, &status_register, 0x01, 2);

    if (status != HAL_OK) {
      if (++count > 10) {
        count = 0;
        goto reinit;
      }
      tx_thread_sleep(2);
      continue;
    }

    count = 0;
    if (!status_register) {
      tx_thread_sleep(2);
      continue;
    }
    status = Bambloo_I2C_Mem_Write(gt911_address, 0x814E, &temp_register, 0x01, 2);
    if (status != HAL_OK) {
      continue;
    }

    if ((status_register & 0x8F) == 0x80) {
      continue;
    }

    status = Bambloo_I2C_Mem_Read(gt911_address, 0x8150, coors, sizeof(coors), 2);

    if (status != HAL_OK) {
      continue;
    }

    uint32_t n_y = coors[0] | coors[1] << 8;
    uint32_t n_x = coors[2] | coors[3] << 8;

    // if (draw) {
    //   app_ltdc_draw_line(n_x, n_y, x, y, 0xFFFF);
    // }

    thread_ltdc_fill_circle(n_x, n_y, 8, 0x00FFFF);
  }
}
