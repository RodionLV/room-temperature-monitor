#ifndef INTERFACE_BLUETOOTH_H
#define INTERFACE_BLUETOOTH_H

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"

#include "esp_log.h"

esp_err_t init_bt_interface();

#endif // INTERFACE_BLUETOOTH_H