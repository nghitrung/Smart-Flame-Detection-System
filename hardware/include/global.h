#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define BUTTON_CONFIG GPIO_NUM_0
#define SMOKE1_PIN A0
#define SMOKE2_PIN A1
#define FLAME1_PIN A2
#define FLAME2_PIN A3
#define SDA 11
#define SCL 12
#define BUZZER 

extern volatile float smoke; // Avg smoke signal 
extern volatile int flame; // Avg flame signal
extern volatile float temp; // Temp signal 

extern volatile bool smoke_alert;

extern SemaphoreHandle_t xMqttMutex;
extern SemaphoreHandle_t xWifiMutex;
extern SemaphoreHandle_t xSmokeMutex;
extern SemaphoreHandle_t xTempMutex;
extern SemaphoreHandle_t xFlameMutex;

#endif