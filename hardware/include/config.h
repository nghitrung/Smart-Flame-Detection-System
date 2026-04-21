#ifndef CONFIG_H
#define CONFIG_H

extern char mqtt_server[40];

// === TOPICS ===
#define TOPIC_VALUE "yolo_uno/sensors/all"

#define MQ2_SMOKE_THRESHOLD 500
#define MQ2_WARMUP_MS 7000UL
#define FLAME_WARMUP_MS 7000UL
#define SMOKE_WARMUP_MS 7000UL
#define TEMP_WARMUP_MS 7000UL
#define MQ2_PPM_SANITY_MAX 10000.0f

#endif