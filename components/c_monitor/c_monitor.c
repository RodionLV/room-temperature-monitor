#include "c_monitor.h"


static const char *TAG = "MONITOR";

static StaticEventGroup_t _monitorEventBuf;
static EventGroupHandle_t monitorEventGroup;


bool is_tem_state_normal();
EventBits_t wait_deviation_tem_state(TickType_t xTicksToWait);

void light_indication_tem_task(void *pvParameters);

void toggle_diode(gpio_num_t gpio_num);


void init_monitor_system() {
    ESP_LOGI(TAG, "initializing monitor system...");
    monitorEventGroup = xEventGroupCreateStatic(&_monitorEventBuf);

    gpio_reset_pin(GPIO_BLUE_DIODE);
    gpio_set_direction(GPIO_BLUE_DIODE, GPIO_MODE_OUTPUT);
  
    gpio_reset_pin(GPIO_ORANGE_DIODE);
    gpio_set_direction(GPIO_ORANGE_DIODE, GPIO_MODE_OUTPUT);
   

    static StaticTask_t xTaskBuffer;
    static StackType_t xStack[2048];

    TaskHandle_t xHandle = 
        xTaskCreateStaticPinnedToCore(
            light_indication_tem_task, "light_indication_tem_task", 2048, NULL, 5, xStack, &xTaskBuffer, 1
        );

    if (xHandle == NULL) {
        ESP_LOGE(TAG, "Failed to task create");
    };
}

void light_indication_tem_task(void *pvParameters) {
    gpio_num_t active_diode = GPIO_BLUE_DIODE;
    uint8_t diode_level = 0;

    EventBits_t state = xEventGroupWaitBits(monitorEventGroup, (EVENT_TEM_TOO_COLD | EVENT_TEM_TOO_WARM | EVENT_TEM_NORMAL), pdFALSE, pdFALSE, portMAX_DELAY);
    if(state == 0){
        ESP_LOGE(TAG, "failed get state about measurement");
        vTaskDelete(NULL);
        return;
    }

    while(true){
        if( is_tem_state_normal() ){
            ESP_LOGI(TAG, "temperature state is normal");

            gpio_set_level(GPIO_BLUE_DIODE, 0);
            gpio_set_level(GPIO_ORANGE_DIODE, 0);
            diode_level = 0;

            EventBits_t bits = wait_deviation_tem_state(portMAX_DELAY);
            
            if( (bits & EVENT_TEM_TOO_COLD) > 0 ) {
                ESP_LOGI(TAG, "temperature state is too cold");
                active_diode = GPIO_BLUE_DIODE;

            }else if( (bits & EVENT_TEM_TOO_WARM) > 0 ) {
                ESP_LOGI(TAG, "temperature state is too warm");
                active_diode = GPIO_ORANGE_DIODE;
            }
        }else{
            diode_level = diode_level ? 0 : 1;
            gpio_set_level(active_diode, diode_level);

            vTaskDelay(TOGGLE_DIODE_DELAY_MS / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
}

bool is_tem_state_normal() {
    return (xEventGroupGetBits(monitorEventGroup) & EVENT_TEM_NORMAL) != 0;
}

EventBits_t wait_deviation_tem_state(TickType_t xTicksToWait) {
    return xEventGroupWaitBits(monitorEventGroup, (EVENT_TEM_TOO_COLD | EVENT_TEM_TOO_WARM), pdFALSE, pdFALSE, xTicksToWait);
}

void set_tem_state(EventBits_t eventBit) {
    xEventGroupClearBits(monitorEventGroup, (EVENT_TEM_TOO_WARM | EVENT_TEM_TOO_COLD | EVENT_TEM_NORMAL));
    xEventGroupSetBits(monitorEventGroup, eventBit);
}

void toggle_diode(gpio_num_t gpio_num) {
    gpio_set_level( gpio_num, (gpio_get_level(gpio_num) ? 0 : 1) );
}