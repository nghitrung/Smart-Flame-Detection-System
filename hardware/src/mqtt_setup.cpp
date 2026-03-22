#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt_setup.h"
#include "global.h"
#include "config.h"

WiFiClient yoloClient;
PubSubClient client(yoloClient);
long lastPublishTime = 0;
const long publishInterval = 3000;

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("\nMessage arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

    if (String(topic) == "home/commands") {
        if (message == "OFF") {
            Serial.println("System is stopped");
        }
    }
}

String packageData(const char* topic) {
    JsonDocument doc;
    char buffer[256];

    if (String(topic) == TOPIC_SMOKE) {
        JsonObject mq2_1_obj = doc["mq2_1"].to<JsonObject>();
        mq2_1_obj["CO"] = smoke[0][0];
        mq2_1_obj["LPG"] = smoke[0][1];
        mq2_1_obj["SMOKE"] = smoke[0][2];

        JsonObject mq2_2_obj = doc["mq2_2"].to<JsonObject>();
        mq2_2_obj["CO"] = smoke[1][0];
        mq2_2_obj["LPG"] = smoke[1][1];
        mq2_2_obj["SMOKE"] = smoke[1][2];
    } else if (String(topic) == TOPIC_FLAME) {
        JsonObject flame_obj = doc["flame_data"].to<JsonObject>();
        flame_obj["FLAME1"] = flame[0];
        flame_obj["FLAME2"] = flame[1];
    }
    serializeJson(doc, buffer);
    client.publish(topic, buffer);

    Serial.printf("\nPublished to %s: %s", topic, buffer);

    return String(buffer);
}

void vTaskMqtt(void* pvParameters) {
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            if (!client.connected()) {
                if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                    Serial.print("\nMQTT: Connecting...");
                    String clientID = "YoloUno-Trung";
                    if (client.connect(clientID.c_str(), "Flame_Detection_System", "123")){
                        Serial.print("\nSUCCESS");
                    } else {
                        Serial.print("\nFAILED, rc= "); Serial.println(client.state());
                    }
                    xSemaphoreGive(xMqttMutex);
                } 
            } else {
                if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                    client.loop();

                    if (millis() - lastPublishTime >= publishInterval) {
                        lastPublishTime = millis();

                        packageData(TOPIC_SMOKE);

                        packageData(TOPIC_FLAME);
                    }

                    xSemaphoreGive(xMqttMutex);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // MQTT beat
    }
}