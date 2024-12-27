#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <nvs_flash.h>

#include <esp_log.h>

#include "config.h"
#include "c_i2c.h"
#include "c_AHT20.h"
#include "c_monitor.h"
#include "c_bluetooth.h"


static const char *TAG = "MAIN"; 

esp_err_t init_nvs() {
    esp_err_t err;

    err = nvs_flash_init();
    if( err == ESP_ERR_NOT_FOUND  || err == ESP_ERR_NVS_NO_FREE_PAGES ){
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    return err;
}

void app_main(void)
{
    esp_err_t err;

    const i2c_master_bus_config_t cfg_bus = i2c_set_master_bus_config();
    
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus( &cfg_bus, &bus_handle));

#if CONFIG_MODE == CONIFG_MODE_ROOM_TEMPERATURE_MONITOR
    ESP_ERROR_CHECK(init_nvs());

    const i2c_device_config_t cfg_device = i2c_set_aht20_config();

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle, 
            &cfg_device,
            &dev_handle
        )
    );

    vTaskDelay(pdMS_TO_TICKS(60)); // wait after power-on

    uint8_t status_aht20 = 0;
    ESP_ERROR_CHECK(i2c_get_status_aht20(&dev_handle, &status_aht20));
    ESP_LOGI(TAG, "status: %x", status_aht20);
    
    vTaskDelay(pdMS_TO_TICKS(20));
    
    if( !is_calibrated_aht20(status_aht20) ){
        ESP_ERROR_CHECK(i2c_init_aht20(&dev_handle));
        ESP_LOGI(TAG, "aht20 is initialized");
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    aht20_measurement_t measurement = {
        .humidity = -1,
        .tempreture = -1,
    };

    init_monitor_system();
    init_bt_interface();

    while(true){
        ESP_LOGI(TAG, "Doing measurement...");
    
        i2c_get_measurement_aht20(&dev_handle, &measurement);
        ESP_LOGI(TAG, "humidity: %.2f%%\ttempreture: %.2fC", measurement.humidity, measurement.tempreture);

        if(measurement.tempreture > 26) {
            set_tem_state(EVENT_TEM_TOO_WARM);
        }else if(measurement.tempreture < 21) {
            set_tem_state(EVENT_TEM_TOO_COLD);
        }else{
            set_tem_state(EVENT_TEM_NORMAL);
        }

        vTaskDelay(DELAY_MESUREMENT_MS / portTICK_PERIOD_MS);
    }
#elif CONFIG_MODE == CONFIG_MODE_FIND_ADDR_DEV 
    i2c_find_addr_dev(&bus_handle);
#endif
}