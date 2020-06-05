#include <Arduino.h>
#include <painlessMesh.h>

#include "config.h"

#define TOGGLE_LIGHT_FREQ 1
#define LIGHT_PIN LED_BUILTIN

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

bool light_status = false;

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

String generateStatusString();

Task taskSendMessage(TASK_SECOND * TOGGLE_LIGHT_FREQ, TASK_FOREVER, &sendMessage);

void sendMessage() {
    light_status = !light_status;
    digitalWrite(LIGHT_PIN, light_status);

    String msg = generateStatusString();
    mesh.sendBroadcast(msg);
}

String generateStatusString() {
    DynamicJsonDocument doc(46);
    JsonObject command = doc.createNestedObject("command");
    command["light"] = light_status;

    String msg;
    serializeJson(doc, msg);
    return msg;
}

void receivedCallback(uint32_t from, String &msg) {
    Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

    DynamicJsonDocument doc(46);
    deserializeJson(doc, msg);

    if (doc.containsKey("command")) {
        JsonObject command = doc["command"];
        if (command.containsKey("light")){
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

    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();
}

void loop() {
    mesh.update();
}