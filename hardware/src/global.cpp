#include "global.h"

volatile float smoke = 0; 
volatile int flame = 0; 
volatile float temp = 0;

volatile bool smoke_alert = false;

SemaphoreHandle_t xMqttMutex = NULL;
SemaphoreHandle_t xWifiMutex = NULL;
SemaphoreHandle_t xFlameMutex = NULL;
SemaphoreHandle_t xSmokeMutex = NULL;
SemaphoreHandle_t xTempMutex = NULL;
