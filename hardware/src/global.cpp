#include "global.h"

volatile float smoke = 0; 
volatile int flame = 0; 
volatile float temp = 0;

volatile bool smoke_alert = false;
volatile bool fire_alert = false;
volatile bool temp_alert = false;
volatile bool led_buzz_on = false;
volatile bool pump_on = false;
volatile bool manual_pump_on = false;
volatile bool manual_alarm_on = false;
volatile bool manual_emergency_on = false;

volatile uint32_t manual_pump_ms = 0;
volatile uint32_t manual_alarm_ms = 0;
volatile uint32_t manual_emergency_ms = 0;

SemaphoreHandle_t xMqttMutex = NULL;
SemaphoreHandle_t xWifiMutex = NULL;
SemaphoreHandle_t xFlameMutex = NULL;
SemaphoreHandle_t xSmokeMutex = NULL;
SemaphoreHandle_t xTempMutex = NULL;
SemaphoreHandle_t xButtonMutex = NULL;
SemaphoreHandle_t xOutputMutex = NULL;

