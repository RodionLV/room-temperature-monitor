#ifndef MONITOR_H
#define MONITOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#define GPIO_BLUE_DIODE GPIO_NUM_18
#define GPIO_RED_DIODE GPIO_NUM_19

#define EVENT_TEM_TOO_WARM BIT0
#define EVENT_TEM_NORMAL BIT1
#define EVENT_TEM_TOO_COLD BIT2

#define EVENT_MODE_MONITOR_NIGHT BIT10
#define EVENT_MODE_MONITOR_DAY BIT11

#define TOGGLE_DIODE_DELAY_MS 1500


void init_monitor_system();
void set_tem_state(EventBits_t eventBit);

#endif // MONITOR_H