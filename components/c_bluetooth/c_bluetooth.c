#include "c_bluetooth.h"

static const char* TAG = "BT_INTERFACE";

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

esp_err_t init_bt_interface(){
    esp_err_t err;
   
    esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&cfg);
    if( err != ESP_OK ){
        ESP_LOGI(TAG, "bt controller init is failed");
        return err;
    }

    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
        if( err != ESP_OK ){
        ESP_LOGI(TAG, "bt controller enable is failed");
        return err;
    }

    err = esp_bluedroid_init();
    if( err != ESP_OK ){
        ESP_LOGI(TAG, "bt bluedroid init is failed");
        return err;
    }

    err = esp_bluedroid_enable();
    if( err != ESP_OK ){
        ESP_LOGI(TAG, "bt bluedroid enable is failed");
        return err;
    }

    esp_ble_gap_set_device_name("esp32-room-indicators");

    err = esp_ble_gap_register_callback(gap_event_handler);
    if( err != ESP_OK ){
        ESP_LOGI(TAG, "ble gap register handler is failed");
        return err;
    }

    return err;
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    ESP_LOGI(TAG, "event: %i", event);

    switch(event){
        default:
        break;
    }
}