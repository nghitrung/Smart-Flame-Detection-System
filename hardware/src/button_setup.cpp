#include "button_setup.h"

void vTaskButton(void* pvParameters) {
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    while (1) {
        if (xOutputMutex != NULL && xSemaphoreTake(xOutputMutex, portMAX_DELAY) == pdPASS) {
            if (digitalRead(BUTTON_PIN) == LOW) {
                delay(50);
                if (digitalRead(BUTTON_PIN) == LOW) {
                    const bool turnOn = !manual_emergency_on;
                    manual_emergency_on = turnOn;
                    manual_pump_on = turnOn;
                    manual_alarm_on = turnOn;
                    manual_emergency_ms = 0;
                    manual_pump_ms = 0;
                    manual_alarm_ms = 0;
                }
            }

            xSemaphoreGive(xOutputMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}