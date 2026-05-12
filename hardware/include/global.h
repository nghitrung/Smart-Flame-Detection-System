#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <stdint.h>
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
#define BUZZER_PIN 8
#define LED_PIN 10
#define PUMP_PIN 38
#define BUTTON_PIN 21

extern volatile float smoke; // Avg smoke signal 
extern volatile int flame; // Avg flame signal
extern volatile float temp; // Temp signal 

extern volatile bool smoke_alert;
extern volatile bool fire_alert;
extern volatile bool temp_alert;
extern volatile bool led_buzz_on;
extern volatile bool pump_on;
extern volatile bool manual_pump_on;
extern volatile bool manual_alarm_on;
extern volatile bool manual_emergency_on;

extern volatile uint32_t manual_pump_ms;
extern volatile uint32_t manual_alarm_ms;
extern volatile uint32_t manual_emergency_ms;

extern SemaphoreHandle_t xMqttMutex;
extern SemaphoreHandle_t xDataMutex;
extern SemaphoreHandle_t xOutputMutex;

#endif