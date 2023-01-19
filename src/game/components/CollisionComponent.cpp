//
// Created by bison on 13-12-22.
//

#include "CollisionComponent.h"

void CollisionComponent::save(json &e) {
    e["collision"] = true;
}
