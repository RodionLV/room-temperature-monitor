#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include <esp_log.h>

#include "config.h"
#include "c_i2c.h"
#include "c_AHT20.h"

static const char *TAG = "MAIN"; 


static uint8_t data_rd[1024];

void app_main(void)
{
    esp_err_t err;

    const i2c_master_bus_config_t cfg_bus = i2c_set_master_bus_config();
    
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus( &cfg_bus, &bus_handle));

#if CONFIG_MODE == CONIFG_MODE_ROOM_TEMPERATURE_MONITOR
    const i2c_device_config_t cfg_device = i2c_set_aht20_config();

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle, 
            &cfg_device,
            &dev_handle
        )
    );

    vTaskDelay(pdMS_TO_TICKS(40)); // wait after power-on

    uint8_t status_aht20 = 0;
    ESP_ERROR_CHECK(i2c_get_status_aht20(&dev_handle, &status_aht20));
    vTaskDelay(pdMS_TO_TICKS(20));
    
    if( !is_calibrated_aht20(status_aht20) ){
        i2c_init_aht20(&dev_handle);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

#elif CONFIG_MODE == CONFIG_MODE_FIND_ADDR_DEV 
    i2c_find_addr_dev(&bus_handle);
#endif
}