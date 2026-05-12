#include "flame_setup.h"

void vTaskFlame(void *pvParameter) {
    pinMode(FLAME1_PIN, INPUT);
    pinMode(FLAME2_PIN, INPUT);

    const uint32_t warmupStart = millis();

    while (1) {
        if (millis() - warmupStart < FLAME_WARMUP_MS) {
            if (xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdPASS) {
                xSemaphoreGive(xDataMutex);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        int rawInput1 = digitalRead(FLAME1_PIN);
        int rawInput2 = digitalRead(FLAME2_PIN);

        if (xDataMutex != NULL && xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdPASS) {
            if (isnan(rawInput1) || isnan(rawInput2)) {
                Serial.print("\nValue is unreadable!"); 
            } else {
                flame = (rawInput1 == LOW || rawInput2 == LOW) ? 1 : 0;
                fire_alert = (flame == 1) ? true : false;

                Serial.printf("\nFlame Detect: %d", flame); 
            }
            xSemaphoreGive(xDataMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}   
