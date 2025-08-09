#ifndef DL_MQTT_H
#define DL_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTHandler {
public:
    void begin(const String& host, int port, const char* hostname, const char* apiKey);
    void loop();
    void publishTag(const String& tag);
    void reconnect();

private:
    static void callback(char* topic, byte* payload, unsigned int length);
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    String _host;
    String _hostname;
    String _apiKey;
    int _reconnectAttempts = 0;
};

extern MQTTHandler mqttHandler;

#endif // DL_MQTT_H
