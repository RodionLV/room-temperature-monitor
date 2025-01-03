#ifndef INTERFACE_BLUETOOTH_H
#define INTERFACE_BLUETOOTH_H

#include <string.h>

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_bt_defs.h"

#include "esp_gap_ble_api.h"

#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"

#include "esp_log.h"


#define GAP_DEVICE_NAME "esp32-room-indicators"

#define INDICATORS_PROFILE_APP_ID       0
#define SERVICE_INST_ID                 0
#define GATTS_INDICATORS_SERVICE_UUID   0x1A00
// #define INDICATORS_HANDLE_NUM           4

#define TEMPRETURE_CHAR_UUID 0x2810
#define TEMPRETURE_DESCR_UUID 0x2811

#define HUMIDITY_CHAR_UUID 0x2812
#define HUMIDITY_DESCR_UUID 0x2813


struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static struct gatts_profile_inst profile_inst;

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,  // 20ms
    .adv_int_max        = 0x40,  // 40ms
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t tempreture_val[4] = {0,0,0,0};
static uint8_t humidity_val[4] = {0,0,0,0};

enum {
    INDEX_SERVICE,

    INDEX_TEMPRETURE_CHAR,
    INDEX_TEMPRETURE_VAL_CHAR,

    INDEX_HUMIDITY_CHAR,
    INDEX_HUMIDITY_VAL_CHAR,

    HRS_IDX_NB
};

esp_err_t init_bt_interface();

void change_tempreture_val_char(uint8_t *val, size_t len);
void change_humidity_val_char(uint8_t *val, size_t len);
// static esp_attr_value_t tempreture_char_attr = {
//     .attr_max_len = 4,
//     .attr_len     = sizeof(tempreture_val),
//     .attr_value   = tempreture_val,
// };

// static uint8_t tempreture_descr[] = {'t', 'e', 'm', 'p', 'r', 'e', 't', 'u', 'r', 'e'};

// static esp_attr_value_t tempreture_descr_attr = {
//     .attr_max_len = sizeof(tempreture_descr),
//     .attr_len     = sizeof(tempreture_descr),
//     .attr_value   = tempreture_descr,
// };

#endif // INTERFACE_BLUETOOTH_H