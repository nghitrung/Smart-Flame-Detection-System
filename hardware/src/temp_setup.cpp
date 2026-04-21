#include "temp_setup.h"

void vTaskTemp(void *pvParameter) {

    Wire.begin(SDA, SCL);

    DHT20 dht20;

    dht20.begin();
    

    const uint32_t warmupStart = millis();

    while (1) {
        if (millis() - warmupStart < TEMP_WARMUP_MS) {
            if (xSemaphoreTake(xTempMutex, portMAX_DELAY) == pdPASS) {
                xSemaphoreGive(xTempMutex);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xTempMutex != NULL && xSemaphoreTake(xTempMutex, portMAX_DELAY) == pdPASS) {
            dht20.read();
            float rawInput = dht20.getTemperature();

            if (isnan(rawInput)) {
                Serial.print("\nValue is unreadable!");
            } else {
                temp = rawInput;
            
                Serial.printf("\nTemp Detect: %.2f", temp);
            }
            xSemaphoreGive(xTempMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

