#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <driver/gpio.h>
#include <driver/i2c_master.h>
#include <esp_log.h>

#include "config.h"

static const char *TAG = "MAIN"; 

#define SCL_SPEED_HZ 100000

#define DEVICE_ADDR_AHT2X 0x38

i2c_master_bus_config_t i2c_set_master_bus_config();
i2c_device_config_t i2c_set_device_config(uint16_t device_address, i2c_addr_bit_len_t dev_addr_length);
uint16_t i2c_find_addr_dev(i2c_master_bus_handle_t *bus_handle);

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

uint16_t i2c_find_addr_dev(i2c_master_bus_handle_t *bus_handle){
    esp_err_t err;
    uint16_t addr_dev = 0;

    for(; addr_dev < 1024; addr_dev++){
        err = i2c_master_probe(*bus_handle, addr_dev, 100 * portTICK_PERIOD_MS);
        if(err == ESP_OK){
            ESP_LOGI(TAG, "addr device: %i", addr_dev);
            break;
        }else if(err == ESP_ERR_NOT_FOUND){
            ESP_LOGI(TAG, "addr is checked: %i", addr_dev);
        }else{
            ESP_ERROR_CHECK(err);
        }
    }

    if(addr_dev >= 1024){
        ESP_LOGE(TAG, "device not found");
    }

    return addr_dev;
}

i2c_device_config_t i2c_set_device_config(uint16_t device_address, i2c_addr_bit_len_t dev_addr_length){
    i2c_device_config_t config = {
        .device_address = device_address,
        .dev_addr_length = dev_addr_length,
        .scl_speed_hz = SCL_SPEED_HZ,
        .scl_wait_us = 50 * portTICK_PERIOD_MS
    };

    return config;
}

i2c_master_bus_config_t i2c_set_master_bus_config(){
    i2c_master_bus_config_t config = {
        .i2c_port = 0,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,

        .clk_source = I2C_CLK_SRC_DEFAULT,

        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    return config;
}