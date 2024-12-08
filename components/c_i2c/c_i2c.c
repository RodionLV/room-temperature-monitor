#include "c_i2c.h"

static const char *TAG = "C_I2C";


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

uint16_t i2c_find_addr_dev(i2c_master_bus_handle_t *bus_handle){
    esp_err_t err;
    uint16_t addr_dev = 0;

    for(; addr_dev < 1024; addr_dev++){
        err = i2c_master_probe(*bus_handle, addr_dev, -1);
        if(err == ESP_OK){
            ESP_LOGI(TAG, "addr device: %i", addr_dev);
            break;
        }else if(err == ESP_ERR_NOT_FOUND){
            ESP_LOGI(TAG, "addr is checked: %i", addr_dev);
        }else{
            ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if(addr_dev >= 1024){
        ESP_LOGE(TAG, "device not found");
    }

    return addr_dev;
}