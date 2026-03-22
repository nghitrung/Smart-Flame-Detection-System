#include "flame_setup.h"
#include "global.h"

void vTaskFlame(void *pvParameter) {
    
    float rawValue1 = analogRead(FLAME1_PIN);
    float rawValue2 = analogRead(FLAME2_PIN);

    while (1) {
        if (xSensorMutex != NULL && xSemaphoreTake(xSensorMutex, portMAX_DELAY) == pdPASS) {
            if (isnan(rawValue1) || isnan(rawValue2)) {
                Serial.print("\nValue is unreadable!"); 
            } else {
                flame[0] = (1.0 - (float)rawValue1/4095.0) * 100;
                flame[1] = (1.0 - (float)rawValue2/4095.0) * 100;

                Serial.printf("\nRaw Value: %.2f | Flame Detect: %.2f", rawValue1, flame[0]); 
                Serial.printf("\nRaw Value: %.2f | Flame Detect: %.2f", rawValue2, flame[1]); 
            }
            xSemaphoreGive(xSensorMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}   
