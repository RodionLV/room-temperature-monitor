#include "c_AHT20.h"

static const char* TAG = "AHT20";

void format_measurement(uint8_t *tem_and_hum, aht20_measurement_t* ret_value);

i2c_device_config_t i2c_set_aht20_config() {
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

esp_err_t i2c_get_status_aht20(i2c_master_dev_handle_t *dev_handle, uint8_t* ret_status) {
    esp_err_t err;

    uint8_t cmd[1] = { AHT20_CMD_STATUS };
    uint8_t data[2] = {0, 0};
   
    err = i2c_master_transmit_receive(*dev_handle, cmd, sizeof(cmd), data, sizeof(data), 40);
    if( err == ESP_OK ){
        *ret_status = data[1];
    }

    return err;
}

esp_err_t i2c_init_aht20(i2c_master_dev_handle_t *dev_handle) {
    uint8_t cmd[3] = { AHT20_CMD_INIT };
    return i2c_master_transmit(*dev_handle, cmd, sizeof(cmd), 40);
}

esp_err_t i2c_get_measurement_aht20(i2c_master_dev_handle_t *dev_handle, aht20_measurement_t *ret_value) {
    esp_err_t err;

    uint8_t cmd[3] = { AHT20_CMD_MEASUREMENT };
    uint8_t data[7] = { 0, 0, 0, 0, 0, 0, 0 };

    err = i2c_master_transmit(*dev_handle, cmd, sizeof(cmd), 100);
    if( err != ESP_OK ){
        return err;
    }

    vTaskDelay(pdMS_TO_TICKS(80));

    uint8_t status = 0;
    do {
        err = i2c_master_receive(*dev_handle, &status, 1, -1);
        if(err != ESP_OK){
            return err;
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    } while ( (status != 0xFF) && is_busy_aht20(status) );
    // TODO: check CRC
    err = i2c_master_receive(*dev_handle, data, sizeof(data), 100);
    if(err == ESP_OK){
        format_measurement(&data[1], ret_value);
    }

    return err;
}

void format_measurement(uint8_t *tem_and_hum, aht20_measurement_t* ret_value) {
    uint32_t dataHum = ((uint32_t)tem_and_hum[0] << 12) | ((uint16_t)tem_and_hum[1] << 4) | (tem_and_hum[2] >> 4);
    uint32_t dataTem = (((uint32_t)tem_and_hum[2] & 0x0F) << 16) | ((uint16_t)tem_and_hum[3] << 8) | tem_and_hum[4];
    
    ret_value->humidity = (float)dataHum / (1<<20) * 100.0;
    ret_value->tempreture = (float)dataTem / (1<<20) * 200.0 - 50.0;
}