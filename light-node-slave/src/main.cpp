#include <Arduino.h>
#include <painlessMesh.h>

#include "config.h"

#define LIGHT_PIN LED_BUILTIN

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
    Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

    DynamicJsonDocument doc(46);
    deserializeJson(doc, msg);

    if (doc.containsKey("command")) {
        JsonObject command = doc["command"];
        if (command.containsKey("light")) {
            digitalWrite(LIGHT_PIN, command["light"]);
        }
    }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
    Serial.begin(115200);
    mesh.setDebugMsgTypes(ERROR | STARTUP);

    pinMode(LIGHT_PIN, OUTPUT);

    mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop() {
    mesh.update();
}