#include "c_AHT20.h"


i2c_device_config_t i2c_set_aht20_config(){
    i2c_device_config_t config = {
        .device_address = AHT20_ADDRESS,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .scl_speed_hz = AHT20_SCL_SPEED_HZ,
        .scl_wait_us = 50 * portTICK_PERIOD_MS
    };

    return config;
}