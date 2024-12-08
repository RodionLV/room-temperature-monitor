#ifndef I2C_MODULE_H
#define I2C_MODULE_H

#include <freertos/FreeRTOS.h>
#include <driver/i2c_master.h>

#include <esp_log.h>


i2c_master_bus_config_t i2c_set_master_bus_config();
uint16_t i2c_find_addr_dev(i2c_master_bus_handle_t *bus_handle);

#endif // I2C_MODULE_H