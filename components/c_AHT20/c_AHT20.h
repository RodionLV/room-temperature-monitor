#ifndef I2C_AHT20_H
#define I2C_AHT20_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c_master.h>
#include <esp_log.h>


#define AHT20_SCL_SPEED_HZ 100000
#define AHT20_ADDRESS 0x38

#define AHT20_CMD_INIT         0xBE, 0x08, 0x00
#define AHT20_CMD_RESET        0xBA
#define AHT20_CMD_MEASUREMENT  0xAC, 0x33, 0x00
#define AHT20_CMD_STATUS       0x71

#define AHT20_STATUS_BIT_CALIBRATED 0x08
#define AHT20_STATUS_BIT_BUSY       0x80

typedef struct {
    float tempreture;
    float humidity;
} aht20_measurement_t;

i2c_device_config_t i2c_set_aht20_config();

bool is_calibrated_aht20(uint8_t status);
bool is_busy_aht20(uint8_t status);

esp_err_t i2c_get_status_aht20(i2c_master_dev_handle_t *dev_handle, uint8_t* ret_status);
esp_err_t i2c_init_aht20(i2c_master_dev_handle_t *dev_handle);
esp_err_t i2c_get_measurement_aht20(i2c_master_dev_handle_t *dev_handle, aht20_measurement_t *ret_value);

#endif // I2C_AHT20_h