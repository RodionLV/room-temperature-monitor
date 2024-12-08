#ifndef I2C_AHT20_H
#define I2C_AHT20_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c_master.h>


#define AHT20_SCL_SPEED_HZ 100000
#define AHT20_ADDRESS 0x38

i2c_device_config_t i2c_set_aht20_config();

#endif // I2C_AHT20_h