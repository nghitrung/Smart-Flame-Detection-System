#include "pump_setup.h"

void vTaskPump(void *pvParamters) {
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);

    while (1) {
        const uint32_t nowMs = millis();
        if (manual_pump_on && manual_pump_ms > 0 && nowMs > manual_pump_ms) {
            manual_pump_on = false;
            manual_pump_ms = 0;
        }
        if (manual_emergency_on && manual_emergency_ms > 0 && nowMs > manual_emergency_ms) {
            manual_emergency_on = false;
            manual_emergency_ms = 0;
        }

        const bool autoTrigger = fire_alert && smoke_alert && temp_alert;
        const bool manualTrigger = manual_pump_on || manual_emergency_on;
        pump_on = autoTrigger || manualTrigger;

        if (xOutputMutex != NULL && xSemaphoreTake(xOutputMutex, portMAX_DELAY) == pdPASS) {
            digitalWrite(PUMP_PIN, pump_on ? HIGH : LOW);
            Serial.printf("\n[PUMP CHECK] pump=%d fire=%d gas=%d temp=%.d manual=%d\n",
                    pump_on, fire_alert, smoke_alert,
                    temp_alert, manualTrigger);
            xSemaphoreGive(xOutputMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}