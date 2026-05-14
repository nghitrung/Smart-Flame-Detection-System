#include "mqtt_setup.h"

WiFiClient yoloClient;
PubSubClient client(yoloClient);
AESLib aesLib;
long lastPublishTime = 0;
const long publishInterval = 3000;
const uint32_t testDurationMs = 10000UL;
const uint32_t emergencyDurationsMs = 20000UL;
const char* DEVICE_ID = "101";

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
    } else if (action == "reset_system") {
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

String decryptData(String encryptText) {
    byte temp_iv[N_BLOCK];
    memcpy(temp_iv, AES_IV, sizeof(AES_IV));

    uint16_t msgLen = encryptText.length();

    char clearText[128];

    aesLib.decrypt64((char *)encryptText.c_str(), msgLen, (byte *)clearText, aes_key, sizeof(aes_key), temp_iv);

    return String(clearText);
}

void callback(char* topic, byte* payload, unsigned int length) {
    if (strcmp(topic, TOPIC_CONTROL) != 0) {
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);

    if (err) {
        Serial.print("\n[CTRL] invalid JSON: ");
        Serial.println(err.c_str());
        return;
    }

    const char* encrypted_cmd = doc["cmd_enc_value"];
    if (encrypted_cmd != NULL) {
        String decrypted_cmd = decryptData(String(encrypted_cmd));
        decrypted_cmd.trim();
        doc.clear();
        DeserializationError decErr = deserializeJson(doc, decrypted_cmd);
        if (decErr) {
            Serial.print("\n[CTRL] Decrypt JSON parse error: ");
            Serial.println(decErr.c_str());
            return;
        }
    }

    if (!doc["roomId"].isNull()) {
        int targetRoomId = doc["roomId"].as<int>();
        int currentRoomId = atoi(DEVICE_ID);
        if (targetRoomId == currentRoomId) {
            if (doc["action"].isNull()) {
                applyControlAction(doc["action"]);
            } else {
                Serial.println("\n[CTRL] Missing 'action' field in JSON");
            }
        } else {
            Serial.printf("\n[CTRL] Ignored: targetRoomId %d does not match local %d\n", targetRoomId, currentRoomId);
            return;
        }
    }

}

String encryptData(String plainText) {
    byte temp_iv[N_BLOCK];
    memcpy(temp_iv, AES_IV, sizeof(AES_IV));

    uint16_t msgLen = plainText.length();

    char cipher64[64];

    aesLib.encrypt64((byte *)plainText.c_str(), msgLen, (char *)cipher64, aes_key, sizeof(aes_key), temp_iv);

    return String(cipher64);
}

void packageData() {
    JsonDocument doc;
    char buffer[512];
    
    doc["room_enc_value"] = encryptData(String(DEVICE_ID));
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
                        if (client.connect(DEVICE_ID, "Flame_Detection_System", "123")){
                            Serial.print("\nSUCCESS");
                            client.subscribe(TOPIC_CONTROL); // Đăng ký nhận lệnh
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

        vTaskDelay(pdMS_TO_TICKS(50)); // MQTT beat
    }
}