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

    // For now, we only care about the reply topic.
    String replyTopic = "dlock/" + mqttHandler._hostname + "/reply";
    if (replyTopic.equals(topic)) {
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

            // Subscribe to reply topic
            String replyTopic = "dlock/" + _hostname + "/reply";
            _mqttClient.subscribe(replyTopic.c_str());
            logger.info("Subscribed to %s", replyTopic.c_str());

            // Publish register message
            String topic = "dlock/" + _hostname + "/register";
            String payload = "{\"api_key\": \"" + _apiKey + "\"}";
            _mqttClient.publish(topic.c_str(), payload.c_str());
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
        reconnect();
    }
    _mqttClient.loop();
}

void MQTTHandler::publishTag(const String& tag) {
    if (_mqttClient.connected()) {
        String topic = "dlock/" + _hostname + "/tag";
        _mqttClient.publish(topic.c_str(), tag.c_str());
    }
}
