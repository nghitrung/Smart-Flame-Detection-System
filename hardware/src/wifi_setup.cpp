#include "wifi_setup.h"
#include "global.h"
#include "config.h"

void vTaskWifi(void* pvParameters) {
    WiFi.begin(ssid, password);
    while (1) {
        if (WiFi.status() == WL_CONNECTED ) {
            if (xWifiMutex != NULL && xSemaphoreTake(xWifiMutex, portMAX_DELAY) == pdPASS) {
                Serial.print("\nWiFi: Connected! | IP: "); Serial.println(WiFi.localIP());
                xSemaphoreGive(xWifiMutex);
            }
        } else {
            if (xWifiMutex != NULL && xSemaphoreTake(xWifiMutex, portMAX_DELAY) == pdPASS) {
                Serial.print("\nWifi is not connected! | Attempting to reconnect!");
                xSemaphoreGive(xWifiMutex);
            }

            WiFi.begin(ssid, password);
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); 
    } 
}
