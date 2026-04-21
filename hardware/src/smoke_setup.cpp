#include "smoke_setup.h"

#define Board "ESP32"
#define Type "MQ-2"
#define Voltage_Res 3.3
#define ADC_Bit_Res 12
#define RatioMQ2CleanAir 9.83

float averagePPM(MQUnifiedsensor& MQ2) {
    float rawValue = 0;
    int validCount = 0;
    for (int i = 0; i < 10; i++) {
        MQ2.update();
        float value = MQ2.readSensor(false, 0);
        if (isfinite(value) && value >= 0 && value <= MQ2_PPM_SANITY_MAX) {
            rawValue += value;
            validCount++;
        }
        vTaskDelay(100);
    }

    float avgValue = (validCount == 10) ? (rawValue / 10) : -1;
    validCount = 0;

    return avgValue;    
}

void vTaskSmoke(void *pvParameter) {

    MQUnifiedsensor MQ2_1(Board, Voltage_Res, ADC_Bit_Res, SMOKE1_PIN, Type);
    MQUnifiedsensor MQ2_2(Board, Voltage_Res, ADC_Bit_Res, SMOKE2_PIN, Type);

    MQ2_1.setRegressionMethod(1); //PPM
    MQ2_2.setRegressionMethod(1); //PPM

    MQ2_1.setA(36974); MQ2_1.setB(-3.109);
    MQ2_2.setA(36974); MQ2_2.setB(-3.109);

    MQ2_1.init();
    MQ2_2.init();

    const uint32_t warmupStart = millis();
    bool isCalibrated = false;

    while (1) {

        if (millis() - warmupStart < MQ2_WARMUP_MS) {
            if (xSemaphoreTake(xSmokeMutex, portMAX_DELAY) == pdPASS) {
                smoke_alert = false;
                xSemaphoreGive(xSmokeMutex);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // calculate Calibrate
        if (!isCalibrated) {
            Serial.print("\nMQ2 Calibrating");

            float calcR0_1 = 0, calcR0_2 = 0;

            for (int i = 0; i < 10; i++) {
                MQ2_1.update();
                MQ2_2.update();
                calcR0_1 += MQ2_1.calibrate(RatioMQ2CleanAir);
                calcR0_2 += MQ2_2.calibrate(RatioMQ2CleanAir);
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            MQ2_1.setR0(calcR0_1 / 10.0);
            MQ2_2.setR0(calcR0_2 / 10.0);


            if (isinf(calcR0_1) || isinf(calcR0_2)) {
                Serial.println("Warning: MQ2 Open Circuit!");
            } else {
                isCalibrated = true;
                Serial.println("MQ2 Calibration Done!");
            }
        
        }
        if (xSmokeMutex != NULL && xSemaphoreTake(xSmokeMutex, portMAX_DELAY) == pdPASS) {
            MQ2_1.update();
            MQ2_2.update();

            float rawInput1 = averagePPM(MQ2_1);
            float rawInput2 = averagePPM(MQ2_2);

            if (rawInput1 == - 1 || rawInput2 == -1) {
                Serial.print("\n Value is unreadable!");
            } else {
                smoke = (rawInput1 + rawInput2) / 2;
                if (rawInput1 > MQ2_SMOKE_THRESHOLD || rawInput2 > MQ2_SMOKE_THRESHOLD) {
                    smoke_alert = true;
                } else {
                    smoke_alert = false;
                }

                Serial.printf("\nSmoke Signal: %.2f ", smoke);
            }
            xSemaphoreGive(xSmokeMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}
