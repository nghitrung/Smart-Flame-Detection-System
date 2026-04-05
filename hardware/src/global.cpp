#include "global.h"

float smoke[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*[{CO_PER1, LPG_PER1, SMOKE_PER1}
                                               {CO_PER2, LPG_PER2, SMOKE_PER2}]*/ 
float flame[2] = {0, 0}; // [flame1, flame2]

SemaphoreHandle_t xMqttMutex = NULL;
SemaphoreHandle_t xWifiMutex = NULL;
SemaphoreHandle_t xSensorMutex = NULL;