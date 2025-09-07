#include "dl_mqtt.h"
#include "dl_config.h"
#include "dl_state.h"
#include "dl_logger.h"
#include "dl_fatal.h"
#include "dl_lcd.h"
#include <ArduinoJson.h>

static Logger logger("MQTT");

MQTTHandler mqttHandler;

void MQTTHandler::callback(char* topic, byte* payload, unsigned int length) {
    logger.info("Message arrived [%s]", topic);

    String cmdTopic = MQTT_TOPIC_BASE + mqttHandler._hostname + "/command";
    if (cmdTopic.equals(topic)) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload, length);

        if (error) {
            logger.error("deserializeJson() failed: %s", error.c_str());
            return;
        }

        const char* action = doc["action"]; // "open" or "deny"
        const char* message = doc["message"];

        if (action && message) {
            logger.info("Action: %s, Message: %s", action, message);
            lcd.displayScrollableMessage(message, 0);
        }
    }
}

void MQTTHandler::begin(const String& host, int port, const char* hostname, const char* apiKey) {
    _host = host;
    _heartbeatMs = 0;
    _hostname = hostname;
    _apiKey = apiKey;
    _mqttClient.setClient(_wifiClient);
    _mqttClient.setServer(_host.c_str(), port);
    _mqttClient.setCallback(callback);
}

void MQTTHandler::reconnect() {
    while (!_mqttClient.connected()) {
        stateHandler.setState(AppState::MQTT_CONNECTING);
        logger.info("Attempting MQTT connection (#%d)...", _reconnectAttempts + 1);
        if (_mqttClient.connect(_hostname.c_str())) {
            logger.info("MQTT connected");
            stateHandler.setState(AppState::MQTT_OK);
            _reconnectAttempts = 0;

            String cmdTopic = MQTT_TOPIC_BASE + _hostname + "/command";
            _mqttClient.subscribe(cmdTopic.c_str());
            logger.info("Subscribed to %s", cmdTopic.c_str());
        } else {
            logger.error("failed, rc=%d", _mqttClient.state());
            _reconnectAttempts++;
            if (_reconnectAttempts >= 5) {
                fatal_error("MQTT Failed!", 30000);
            }
            delay(5000);
        }
    }
}

void MQTTHandler::loop() {
    if (!_mqttClient.connected()) {
        stateHandler.setState(AppState::MQTT_CONNECTING);
        reconnect();
    } else if (stateHandler.getState() == AppState::MQTT_CONNECTING) {
        stateHandler.setState(AppState::MQTT_OK);
    }

    if (millis() - _heartbeatMs > 5000) {
        publishHeartbeat();
        _heartbeatMs = millis();
    }

    _mqttClient.loop();
}

void MQTTHandler::publishHeartbeat() {
    if (_mqttClient.connected()) {
        String topic = MQTT_TOPIC_BASE + _hostname + "/events/heartbeat";
        JsonDocument doc;
        doc["timestamp"] = (uint32_t)(millis() / 1000);
        String payload;
        serializeJson(doc, payload);
        _mqttClient.publish(topic.c_str(), payload.c_str());
    }
}

void MQTTHandler::publishTag(const String& tag) {
    if (_mqttClient.connected()) {
        String topic = MQTT_TOPIC_BASE + _hostname + "/events/rfid";
        JsonDocument doc;
        doc["tag"] = tag;
        doc["timestamp"] = (uint32_t)(millis() / 1000);
        String payload;
        serializeJson(doc, payload);
        _mqttClient.publish(topic.c_str(), payload.c_str());
    }
}

void MQTTHandler::publishButtonPress() {
    if (_mqttClient.connected()) {
        String topic = MQTT_TOPIC_BASE + _hostname + "/events/button";
        JsonDocument doc;
        doc["timestamp"] = (uint32_t)(millis() / 1000);
        String payload;
        serializeJson(doc, payload);
        logger.info("Publishing to %s: %s", topic.c_str(), payload.c_str());
        _mqttClient.publish(topic.c_str(), payload.c_str());
    }
}