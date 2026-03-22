#include <Arduino.h>
#include "global.h"
#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "config.h"
#include "smoke_setup.h"
#include "flame_setup.h"

void setup() {
    Serial.begin(115200);

    delay(3000);

    xWifiMutex = xSemaphoreCreateMutex();
    xMqttMutex = xSemaphoreCreateMutex();
    xSensorMutex = xSemaphoreCreateMutex();

    if (xWifiMutex != NULL && xMqttMutex != NULL && xSensorMutex != NULL) {
        xTaskCreate(vTaskWifi, "WiFi_Task", 4096, NULL, 3, NULL);
        
        xTaskCreate(vTaskMqtt, "MQTT_Task", 4096, NULL, 3, NULL);

        xTaskCreate(vTaskSmoke, "Smoke_Task", 4096, NULL, 1, NULL);

        xTaskCreate(vTaskFlame, "Flame_Task", 4096, NULL, 1, NULL);
        
        Serial.println("System is working!");
    } else {
        Serial.println("Failed to create mutex!");
    }
}

void loop() {
    return;
}