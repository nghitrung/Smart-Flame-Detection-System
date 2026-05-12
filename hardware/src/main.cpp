#include <Arduino.h>
#include "global.h"
#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "config.h"
#include "smoke_setup.h"
#include "flame_setup.h"
#include "temp_setup.h"
#include "button_setup.h"
#include "buzz_setup.h"
#include "led_setup.h"
#include "pump_setup.h"

void setup() {
    Serial.begin(115200);

    delay(3000);
    
    xMqttMutex = xSemaphoreCreateMutex();
    xDataMutex = xSemaphoreCreateMutex();
    xOutputMutex = xSemaphoreCreateMutex();

    if (xMqttMutex != NULL && xDataMutex != NULL && xOutputMutex != NULL) {
        xTaskCreate(vTaskWifi, "WiFi_Task", 4096, NULL, 4, NULL);
        
        xTaskCreate(vTaskMqtt, "MQTT_Task", 4096, NULL, 3, NULL);

        xTaskCreate(vTaskSmoke, "Smoke_Task", 4096, NULL, 1, NULL);

        xTaskCreate(vTaskFlame, "Flame_Task", 4096, NULL, 1, NULL);

        xTaskCreate(vTaskTemp, "Temp_Task", 4096, NULL, 1, NULL);

        xTaskCreate(vTaskButton, "Button_Task", 2048, NULL, 2, NULL);
        
        xTaskCreate(vTaskBuzz, "Buzz_Task", 2048, NULL, 2, NULL);
        
        xTaskCreate(vTaskLed, "Led_Task", 2048, NULL, 2, NULL);

        xTaskCreate(vTaskPump, "Pump_Task", 2048, NULL, 2, NULL);
        
        Serial.println("System is working!");
    } else {
        Serial.println("Failed to create mutex!");
    }
}

void loop() {
    return;
}