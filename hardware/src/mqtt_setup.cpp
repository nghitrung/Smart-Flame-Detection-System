#include "mqtt_setup.h"

WiFiClient yoloClient;
PubSubClient client(yoloClient);
AESLib aesLib;
long lastPublishTime = 0;
const long publishInterval = 3000;
const uint32_t testDurationMs = 10000UL;
const uint32_t emergencyDurationsMs = 20000UL;

byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                   0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
byte AES_IV[N_BLOCK] = { 0x28, 0x34, 0xA5, 0xAF, 0xBE, 0xB8, 0x14, 0xF5,
                         0x08, 0xE1, 0x24, 0xD2, 0xB3, 0xAB, 0xDB, 0xCE };

static void applyControlAction(const String& action) {
    const uint32_t nowMs = millis();
    if (action == "pump_on") {
        manual_pump_on = true;
        manual_pump_ms = nowMs + testDurationMs;
        Serial.println("\n[CTRL] pump_on accepted");
    } else if (action == "pump_off") {
        manual_pump_on = false;
        manual_pump_ms = 0;
        Serial.println("\n[CTRL] pump_off accepted");
    } else if (action == "test_alarm") {
        manual_alarm_on = true;
        manual_alarm_ms = nowMs + testDurationMs;
        Serial.println("\n[CTRL] test_alarm accepted");
    } else if (action == "rest_system") {
        manual_pump_on = false;
        manual_alarm_on = false;
        manual_emergency_on = false;
        manual_pump_ms = 0;
        manual_alarm_ms = 0;
        manual_emergency_ms = 0;
        fire_alert = false;
        smoke_alert = false;
        temp_alert = false;
        Serial.println("\n[CTRL] reset_system accepted");
    } else if (action == "full_test") {
        manual_pump_on = true;
        manual_alarm_on = true;
        manual_pump_ms = nowMs + testDurationMs;
        manual_alarm_ms = nowMs + testDurationMs;
        Serial.println("\n[CTRL] full_test accepted");
    } else if (action == "emergency_alert") {
        manual_emergency_on = true;
        manual_pump_on = true;
        manual_alarm_on = true;
        manual_emergency_ms = nowMs + emergencyDurationsMs;
        manual_pump_ms = nowMs + emergencyDurationsMs;
        manual_alarm_ms = nowMs + emergencyDurationsMs;
        Serial.println("\n[CTRL] emergency_alert accepted");
    } else if (action == "emergency_off" || action == "auto_mode" || action == "all_outputs_off") {
        manual_emergency_on = false;
        manual_pump_on = false;
        manual_alarm_on = false;
        manual_emergency_ms = 0;
        manual_pump_ms = 0;
        manual_alarm_ms = 0;
        Serial.println(action == "all_outputs_off"
                           ? "\n[CTRL] all_outputs_off: LED/buzzer/pump off, AUTO"
                           : "\n[CTRL] emergency_off accepted, back to AUTO");
    } else {
        Serial.print("\n[CTRL] unknown action: ");
        Serial.println(action);
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    if (String(topic) != TOPIC_CONTROL) {
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);

    if (err) {
        Serial.print("\n[CTRL] invalid JSON: ");
        Serial.println(err.c_str());
        return;
    }

    const char* action = doc["action"];
    if (action == NULL) {
        Serial.println("\n[CTRL] missing action field");
        return;
    }
    applyControlAction(String(action));
}

String encryptData(String plainText) {
    byte temp_iv[N_BLOCK];
    memcpy(temp_iv, AES_IV, sizeof(AES_IV));

    uint16_t msgLen = plainText.length();

    char cipher64[64];

    aesLib.encrypt64((byte *)plainText.c_str(), msgLen, cipher64, aes_key, sizeof(aes_key), temp_iv);

    return String(cipher64);
}

void packageData() {
    JsonDocument doc;
    char buffer[512];

    doc["smoke_enc_value"] = encryptData(String(smoke));
    doc["flame_enc_value"] = encryptData(String(flame));
    doc["temp_enc_value"] = encryptData(String(temp));

    serializeJson(doc, buffer);
    client.publish(TOPIC_VALUE, buffer);

    Serial.printf("\n Published with hash: %s", buffer);
}

void vTaskMqtt(void* pvParameters) {
    client.setCallback(callback);

    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            if (!client.connected()) {
                if (String(mqtt_server) != "NO_IP") {
                    if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                        Serial.printf("\nMQTT: Connecting to %s...", mqtt_server);
                        client.setServer(mqtt_server, 1883);
                        String clientID = "YoloUno-Trung";
                        if (client.connect(clientID.c_str(), "Flame_Detection_System", "123")){
                            Serial.print("\nSUCCESS");
                        } else {
                            Serial.print("\nFAILED, rc= "); Serial.println(client.state());
                        }
                        xSemaphoreGive(xMqttMutex);
                    }
                } else {
                    Serial.print("\nWarning: Broker IP not configured yet!");
                }  
            } else {
                if (xMqttMutex != NULL && xSemaphoreTake(xMqttMutex, portMAX_DELAY) == pdPASS) {
                    client.loop();

                    if (millis() - lastPublishTime >= publishInterval) {
                        lastPublishTime = millis();

                        packageData();
                    }

                    xSemaphoreGive(xMqttMutex);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); // MQTT beat
    }
}