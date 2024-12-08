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

    const i2c_master_bus_config_t cfg_bus = 
        i2c_set_master_bus_config();
    
    i2c_master_bus_handle_t bus_handle;
    err = i2c_new_master_bus( &cfg_bus, &bus_handle);
    if( err != ESP_OK){
        ESP_LOGE(TAG, "i2c_new_master_bus: %d", err);
        return;
    }   

#if CONFIG_MODE == CONFIG_MODE_FIND_ADDR_DEV 
    i2c_find_addr_dev(&bus_handle);
#elif CONFIG_MODE == CONIFG_MODE_ROOM_TEMPERATURE_MONITOR
    const i2c_device_config_t cfg_device = 
        i2c_set_device_config(
            DEVICE_ADDR_AHT2X,
            I2C_ADDR_BIT_LEN_7
        );

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle, 
            &cfg_device, 
            &dev_handle
        )
    );

    while(1){
        err = i2c_master_receive(dev_handle, data_rd, 1024, -1);
        if( err != ESP_OK ){
            ESP_LOGE(TAG, "error: %d", err);
        }

        ESP_LOGI(TAG, "read: %s i%i x%x | i%i x%x", data_rd, data_rd[0], data_rd[0], data_rd[1], data_rd[1]);
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
#endif
}