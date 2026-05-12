#include "temp_setup.h"

void vTaskTemp(void *pvParameter) {

    Wire.begin(SDA, SCL);

    DHT20 dht20;

    dht20.begin();
    

    const uint32_t warmupStart = millis();

    while (1) {
        if (millis() - warmupStart < TEMP_WARMUP_MS) {
            if (xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdPASS) {
                xSemaphoreGive(xDataMutex);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        dht20.read();
        float rawInput = dht20.getTemperature();

        if (xDataMutex != NULL && xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdPASS) {
            if (isnan(rawInput)) {
                Serial.print("\nValue is unreadable!");
            } else {
                temp = rawInput;
                temp_alert = (temp > TEMP_SANITY_MAX) ? true : false;
                Serial.printf("\nTemp Detect: %.2f", temp);
            }
            xSemaphoreGive(xDataMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

