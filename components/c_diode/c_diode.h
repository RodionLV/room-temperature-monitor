#ifndef LIGHT_DIODE_H
#define LIGHT_DIODE_h

#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void blink_diode(gpio_num_t gpio_num, uint16_t delay_ms);

#endif // LIGHT_DIODE_h