#include "c_bluetooth.h"

static const char* TAG = "BT_INTERFACE";

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static uint16_t gatt_app_handles[HRS_IDX_NB];

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;

static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static const uint16_t gatt_indicators_service_uuid = GATTS_INDICATORS_SERVICE_UUID;
static const uint16_t gatt_tempreture_char_uuid = TEMPRETURE_CHAR_UUID;
static const uint16_t gatt_humidity_char_uuid = HUMIDITY_CHAR_UUID;

static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] = {
    [INDEX_SERVICE] = {
        {ESP_GATT_AUTO_RSP}, 
        {
            ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, 
            ESP_GATT_PERM_READ, 
            sizeof(uint16_t), sizeof(GATTS_INDICATORS_SERVICE_UUID), (uint8_t *)&gatt_indicators_service_uuid
        }
    },

    [INDEX_TEMPRETURE_CHAR] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, 
            ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read_notify
        }
    },

    [INDEX_TEMPRETURE_VAL_CHAR] = {
        {ESP_GATT_AUTO_RSP}, 
        {
            ESP_UUID_LEN_16, (uint8_t *)&gatt_tempreture_char_uuid, ESP_GATT_PERM_READ,
            4, sizeof(tempreture_val), tempreture_val
        }
    },

    [INDEX_HUMIDITY_CHAR] = {
        {ESP_GATT_AUTO_RSP},
        {
            ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, 
            ESP_GATT_PERM_READ,
            sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read_notify
        }
    },

    [INDEX_HUMIDITY_VAL_CHAR] = {
        {ESP_GATT_AUTO_RSP}, 
        {
            ESP_UUID_LEN_16, (uint8_t *)&gatt_humidity_char_uuid, ESP_GATT_PERM_READ,
            4, sizeof(humidity_val), humidity_val
        }
    }
};


esp_err_t init_bt_interface(){
    esp_err_t err;
   
    esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&cfg);
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "bt controller init is failed");
        return err;
    }

    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
        if( err != ESP_OK ){
        ESP_LOGE(TAG, "bt controller enable is failed");
        return err;
    }

    err = esp_bluedroid_init();
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "bt bluedroid init is failed");
        return err;
    }

    err = esp_bluedroid_enable();
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "bt bluedroid enable is failed");
        return err;
    }

    esp_ble_gap_set_device_name(GAP_DEVICE_NAME);

    err = esp_ble_gap_register_callback(gap_event_handler);
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "ble gap register handler is failed");
        return err;
    }

    profile_inst.gatts_cb = gatts_event_handler;
    profile_inst.gatts_if = ESP_GATT_IF_NONE;

    err = esp_ble_gatts_register_callback(gatts_event_handler);
    if( err != ESP_OK ){
        ESP_LOGE(TAG, "ble gatts regiser handler is failed");
        return err;
    }

    err = esp_ble_gatts_app_register(INDICATORS_PROFILE_APP_ID);
    if (err) {
        ESP_LOGE(TAG, "gatts app register failed");
        return err;
    }

    err = esp_ble_gatt_set_local_mtu(500);
    if (err) {
        ESP_LOGE(TAG, "set local  MTU failed");
        return err;
    }

    return err;
}

void change_tempreture_val_char(uint8_t *val, size_t len) {
    memcpy(tempreture_val, val, len);
    esp_err_t err = esp_ble_gatts_set_attr_value(gatt_app_handles[INDEX_TEMPRETURE_VAL_CHAR], len, tempreture_val);
    if(err){
        ESP_LOGI(TAG, "set attr value is failed");
        return;
    }
    // esp_ble_gatts_send_service_change_indication(profile_inst.gatts_if, profile_inst.conn_id);
}

void change_humidity_val_char(uint8_t *val, size_t len) {
    memcpy(humidity_val, val, len);
    esp_err_t err = esp_ble_gatts_set_attr_value(gatt_app_handles[INDEX_HUMIDITY_VAL_CHAR], len, humidity_val);
    if(err){
        ESP_LOGI(TAG, "set attr value is failed");
        return;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    ESP_LOGI(TAG, "gap event: %i", event);
    esp_err_t err;

    switch(event){
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "gap adv data set complete");

        err = esp_ble_gap_start_advertising(&adv_params);
        if(err){
            ESP_LOGE(TAG, "ble gap start advertising is failed");
            break;
        }
        break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(TAG, "gatts event: %i", event);
    esp_err_t err;

    switch(event){
        case ESP_GATTS_REG_EVT:
        ESP_LOGI(TAG, "gatts server registered, with status: %d and app_id: %d", param->reg.status, param->reg.app_id);
        profile_inst.gatts_if = gatts_if;
        // profile_inst.service_id.is_primary = true;
        // profile_inst.service_id.id.inst_id = 0x00;
        // profile_inst.service_id.id.uuid.len = ESP_UUID_LEN_16;
        // profile_inst.service_id.id.uuid.uuid.uuid16 = GATTS_INDICATORS_SERVICE_UUID;
    
        err = esp_ble_gap_config_adv_data(&adv_data);
        if (err) {
            ESP_LOGE(TAG, "config adv data is failed");
            break;
        }

        // err = esp_ble_gatts_create_service(gatts_if, &profile_inst.service_id, INDICATORS_HANDLE_NUM);
        // if (err) {
        //     ESP_LOGE(TAG, "gatts create service is failed");
        //     break;
        // }
      
        err = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SERVICE_INST_ID);
        if (err){
            ESP_LOGE(TAG, "create attr table failed, error code = %x", err);
            break;
        }
        break;
        case ESP_GATTS_READ_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
        // esp_gatt_rsp_t rsp;
        // memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        // rsp.attr_value.handle = param->read.handle;
        // rsp.attr_value.len = sizeof(tempreture_val);
        // memcpy(&rsp.attr_value, &tempreture_val, rsp.attr_value.len);

        // esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
        case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(TAG, "Service create, status %d, service_handle %d", param->create.status, param->create.service_handle);
        profile_inst.service_handle = param->create.service_handle;
        // profile_inst.char_uuid.len = ESP_UUID_LEN_16;
        // profile_inst.char_uuid.uuid.uuid16 = TEMPRETURE_CHAR_UUID;
    
        // err = esp_ble_gatts_start_service(profile_inst.service_handle);
        // if (err) {
        //     ESP_LOGE(TAG, "gatts start service is failed");
        //     break;
        // }

        // err = esp_ble_gatts_add_char(
        //         profile_inst.service_handle, 
        //         &profile_inst.char_uuid,
        //         ESP_GATT_PERM_READ,
        //         ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
        //         &tempreture_char_attr, 
        //         NULL
        //     );
        
        // if (err) {
        //     ESP_LOGE(TAG, "gatts add char is failed");
        //     break;
        // }   
        break;
        case ESP_GATTS_ADD_CHAR_EVT:
        ESP_LOGI(TAG, "Characteristic add, status %d, attr_handle %d, char_uuid %x", param->add_char.status, param->add_char.attr_handle, param->add_char.char_uuid.uuid.uuid16);
        // profile_inst.char_handle = param->add_char.attr_handle;
        // profile_inst.descr_uuid.len = ESP_UUID_LEN_16;
        // profile_inst.descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

        // err = esp_ble_gatts_add_char_descr(
        //             profile_inst.service_handle, 
        //             &profile_inst.descr_uuid,
        //             ESP_GATT_PERM_READ,
        //             NULL, // &tempreture_descr_attr
        //             NULL
        //         );
        // if(err){
        //     ESP_LOGE(TAG, "gatts add descriptor is failed");
        //     break;
        // }
        break;
        case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        ESP_LOGI(TAG, "Descriptor add, status %d, attr_handle %d, char_uuid %x", param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.descr_uuid.uuid.uuid16);
        // profile_inst.descr_handle = param->add_char_descr.attr_handle;
        break;
        case ESP_GATTS_START_EVT:
        break;
        case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(TAG, "Connected, conn_id %d, remote "ESP_BD_ADDR_STR"", param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        profile_inst.conn_id = param->connect.conn_id;
        break;
        case ESP_GATTS_DISCONNECT_EVT:
        err = esp_ble_gap_start_advertising(&adv_params);
        if(err){
            ESP_LOGE(TAG, "ble gap start advertising is failed");
            break;
        }
        break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != HRS_IDX_NB) {
            ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) \
                    doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
        } else {
            ESP_LOGI(TAG, "create attribute table successfully, the number handle = %d",param->add_attr_tab.num_handle);
            memcpy(gatt_app_handles, param->add_attr_tab.handles, sizeof(gatt_app_handles));
            esp_ble_gatts_start_service(gatt_app_handles[INDEX_SERVICE]);
            // esp_ble_gatts_show_local_database();
        }
        break;
        case ESP_GATTS_SET_ATTR_VAL_EVT: 
        default:
        break;
    }
}