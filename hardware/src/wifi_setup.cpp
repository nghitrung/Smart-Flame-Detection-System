#include <WiFiManager.h>
#include <Preferences.h>
#include "wifi_setup.h"
#include "global.h"
#include "config.h" 

Preferences prefs;
char mqtt_server[40];

WiFiManagerParameter custom_mqtt_server("server", "MQTT Server IP", mqtt_server, 40);

WiFiManager wm;

void saveConfigCallback() {
    strcpy(mqtt_server, custom_mqtt_server.getValue());

    prefs.begin("mqtt_config", false);
    prefs.putString("mqtt_server", mqtt_server);
    prefs.end();

    Serial.printf("\n IP is saved into Flash: %s \n", mqtt_server);
}

void vTaskWifi(void* pvParameters) {

    // Open mqtt_cfg (like a dictionary), True = Read-only mode 
    prefs.begin("mqtt_config", true);
    String new_ip = prefs.getString("mqtt_server", "NO_IP");

    if (new_ip == "NO_IP") {
        Serial.println("Warning: Configuration IP is failed");
    } else {
        strcpy(mqtt_server, new_ip.c_str());
    }

    prefs.end();

    wm.setDarkMode(true);
    wm.setTitle("Flame-Detection-System");
    wm.addParameter(&custom_mqtt_server);
    wm.setSaveConfigCallback(saveConfigCallback);
    wm.setConfigPortalTimeout(180);

    if (!wm.autoConnect("Yolo_Fire_System", "12345678")) {
        Serial.println("Connection Failed!");
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP.restart();
    } 

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    while (1) {
        if (digitalRead(BUTTON_CONFIG) == LOW) {
            delay(3000); // Hold 3 second
            if (digitalRead(BUTTON_CONFIG) == LOW) {
                Serial.println("Reset Yolo!");
                wm.resetSettings();
                ESP.restart();
            }
        }
        
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.reconnect();
            vTaskDelay(pdMS_TO_TICKS(5000));
        } else {
            vTaskDelay(pdMS_TO_TICKS(2000)); 
        }
    } 
}
