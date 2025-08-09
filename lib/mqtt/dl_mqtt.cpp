#include "dl_mqtt.h"
#include "dl_config.h"
#include "dl_state.h"
#include "dl_logger.h"
#include "dl_fatal.h"

static Logger logger("MQTT");

MQTTHandler mqttHandler;

void MQTTHandler::begin(const String& host, int port, const char* hostname, const char* apiKey) {
    _host = host;
    _hostname = hostname;
    _apiKey = apiKey;
    _mqttClient.setClient(_wifiClient);
    _mqttClient.setServer(_host.c_str(), port);
}

void MQTTHandler::reconnect() {
    while (!_mqttClient.connected()) {
        stateHandler.setState(AppState::MQTT_CONNECTING);
        logger.info("Attempting MQTT connection (#%d)...", _reconnectAttempts + 1);
        if (_mqttClient.connect(_hostname.c_str())) {
            logger.info("MQTT connected");
            stateHandler.setState(AppState::MQTT_OK);
            _reconnectAttempts = 0;
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
