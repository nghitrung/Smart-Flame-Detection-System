#include "buzz_setup.h"

BlinkState buzzerAlarmState;

void vTaskBuzz(void* pvParameter) {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    while (1) {
        if (xOutputMutex != NULL && xSemaphoreTake(xOutputMutex, portMAX_DELAY) == pdPASS) {
            const uint32_t nowMs = millis(); 
            if (manual_alarm_on && manual_alarm_ms > 0 && nowMs > manual_alarm_ms) {
                manual_alarm_on = false;
                manual_alarm_ms = 0;
            }

            if (manual_emergency_on && manual_emergency_ms > 0 && nowMs > manual_emergency_ms) {
                manual_emergency_on = false;
                manual_emergency_ms = 0;
            }

            const bool autoTrigger = fire_alert && smoke_alert && temp_alert;
            const bool trigger = autoTrigger || manual_alarm_on || manual_emergency_on;

            bool emergencyBuzzerState = updateTrigger(trigger, buzzerAlarmState, 300UL, 350UL);

            digitalWrite(BUZZER_PIN, emergencyBuzzerState ? HIGH : LOW);

            xSemaphoreGive(xOutputMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}