#include "flame_setup.h"

void vTaskFlame(void *pvParameter) {
    pinMode(FLAME1_PIN, INPUT);
    pinMode(FLAME2_PIN, INPUT);

    const uint32_t warmupStart = millis();

    while (1) {
        if (millis() - warmupStart < FLAME_WARMUP_MS) {
            if (xSemaphoreTake(xFlameMutex, portMAX_DELAY) == pdPASS) {
                xSemaphoreGive(xFlameMutex);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }


        if (xFlameMutex != NULL && xSemaphoreTake(xFlameMutex, portMAX_DELAY) == pdPASS) {
            int rawInput1 = digitalRead(FLAME1_PIN);
            int rawInput2 = digitalRead(FLAME2_PIN);
            if (isnan(rawInput1) || isnan(rawInput2)) {
                Serial.print("\nValue is unreadable!"); 
            } else {
                if (rawInput1 == LOW || rawInput2 == LOW) {
                    flame = 1;
                } else {
                    flame = 0;
                }

                Serial.printf("\nFlame Detect: %d", flame); 
            }
            xSemaphoreGive(xFlameMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}   
