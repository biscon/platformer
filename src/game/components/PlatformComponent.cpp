//
// Created by bison on 10-01-23.
//

#include "PlatformComponent.h"

PlatformComponent::PlatformComponent(const json& e) {
    size = Vector2(e["size"][0], e["size"][1]);
    speed = e["speed"];
    waitTime = e["waitTime"];
    easeAmount = e["easeAmount"];
}

void PlatformComponent::save(json &e) {
    json t;
    pushPos(t["size"], size);
    t["speed"] = speed;
    t["waitTime"] = waitTime;
    t["easeAmount"] = easeAmount;
    e["platform"] = t;
}
