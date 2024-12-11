#include "c_AHT20.h"

static const char* TAG = "AHT20";


i2c_device_config_t i2c_set_aht20_config(){
    i2c_device_config_t config = {
        .device_address = AHT20_ADDRESS,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = AHT20_SCL_SPEED_HZ,
        .scl_wait_us = 0
    };

    return config;
}

bool is_calibrated_aht20(uint8_t status){
    return (status & AHT20_STATUS_BIT_CALIBRATED) != 0;
}

bool is_busy_aht20(uint8_t status){
    return (status & AHT20_STATUS_BIT_BUSY) != 0;
}

esp_err_t i2c_get_status_aht20(i2c_master_dev_handle_t *dev_handle, uint8_t* ret_status){
    esp_err_t err;

    uint8_t cmd[1] = { AHT20_CMD_STATUS };
    uint8_t data[2] = {0, 0};
   
    ESP_LOGI(TAG, "checking status of AHT20...");
    err = i2c_master_transmit_receive(*dev_handle, cmd, sizeof(cmd), data, sizeof(data), 40);
    if( err == ESP_OK ){
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, 2, ESP_LOG_INFO);
        *ret_status = data[1];
    }

    return err;
}

esp_err_t i2c_init_aht20(i2c_master_dev_handle_t *dev_handle){
    esp_err_t err;

    uint8_t cmd[3] = { AHT20_CMD_INIT };
    uint8_t data[2] = {0, 0};

    ESP_LOGI(TAG, "initializing AHT20...");
    err = i2c_master_transmit_receive(*dev_handle, cmd, sizeof(cmd), data, sizeof(data), 40);
    if( err == ESP_OK ){
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, 2, ESP_LOG_DEBUG);
    }

    return err;
}