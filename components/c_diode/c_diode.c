#include "c_diode.h"


void blink_diode(gpio_num_t gpio_num, uint16_t delay_ms) {
    uint8_t gpio_level = 0;

    while(true){
        gpio_level = gpio_level ? 0 : 1;
        gpio_set_level(gpio_num, gpio_level);

        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
    }
}