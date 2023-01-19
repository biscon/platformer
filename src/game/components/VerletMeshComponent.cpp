//
// Created by bison on 08-01-23.
//

#include "VerletMeshComponent.h"

VerletStick::VerletStick(u32 indexA, u32 indexB, bool lockLength, std::vector<VerletPoint>& points) : indexA(indexA),
                                                                     indexB(indexB), lockLength(lockLength) {
    length = points[indexA].position.distance(points[indexB].position);
}

VerletMeshComponent::VerletMeshComponent(const json &e) {
    gravity = e["gravity"];
    solveIterations = e["solveIterations"];
    attachOffset = Vector2(e["attachOffset"][0], e["attachOffset"][1]);
    points.clear();
    for(auto& jsonP : e["points"]) {
        points.emplace_back(VerletPoint(Vector2(jsonP["position"][0], jsonP["position"][1]), jsonP["locked"]));
    }

    sticks.clear();
    u32 i = 0;
    for(auto& jsonS : e["sticks"]) {
        sticks.emplace_back(VerletStick(jsonS["indexA"], jsonS["indexB"], jsonS["lockLength"], points));
        order.emplace_back(i);
        i += 1;
    }

    i = (u32) points.size()-5;
    topLeft = points[i+1].position;
    topRight = points[i+2].position;
    bottomLeft = points[i+3].position;
    bottomRight = points[i+4].position;
}

void VerletMeshComponent::buildRope(Vector2 translate) {
    points.emplace_back(VerletPoint(translate, true));

    for(u32 i = 1; i < 10; i++) {
        translate += Vector2(20,0);
        points.emplace_back(VerletPoint(translate, false));
    }

    float imageW = 4 * 10;
    float imageH = 200;

    points.emplace_back(VerletPoint(translate + Vector2(-imageW,20), false)); // top left
    points.emplace_back(VerletPoint(translate + Vector2(imageW,20), false));  // top right
    points.emplace_back(VerletPoint(translate + Vector2(-imageW,imageH+20), false)); // bottom left
    points.emplace_back(VerletPoint(translate + Vector2(imageW, imageH+20), false));  // bottom right


    for(u32 i = 0; i < points.size()-4; i++) {
        if(i+1 < points.size()-4) {
            sticks.emplace_back(VerletStick(i, i + 1, false, points));
            order.emplace_back(i);
        }
    }

    u32 i = (u32) points.size()-5;
    sticks.emplace_back(VerletStick(i, i+1, true, points));
    order.emplace_back(sticks.size()-1);
    sticks.emplace_back(VerletStick(i, i+2, true, points));
    order.emplace_back(sticks.size()-1);
    sticks.emplace_back(VerletStick(i+1, i+2, true, points));
    order.emplace_back(sticks.size()-1);
    sticks.emplace_back(VerletStick(i+1, i+3, true, points));
    order.emplace_back(sticks.size()-1);
    sticks.emplace_back(VerletStick(i+2, i+4, true, points));
    order.emplace_back(sticks.size()-1);
    sticks.emplace_back(VerletStick(i+3, i+4, true, points));
    order.emplace_back(sticks.size()-1);
    sticks.emplace_back(VerletStick(i+1, i+4, true, points));
    order.emplace_back(sticks.size()-1);
    //sticks.emplace_back(VerletStick(points[i+2], points[i+3], true));

    topLeft = points[i+1].position;
    topRight = points[i+2].position;
    bottomLeft = points[i+3].position;
    bottomRight = points[i+4].position;
}

void VerletMeshComponent::save(json &e) {
    json t;
    t["gravity"] = gravity;
    t["solveIterations"] = solveIterations;
    pushPos(t["attachOffset"], attachOffset);
    
    for(auto& p : points) {
        json jsonP;
        pushPos(jsonP["position"], p.position);
        jsonP["locked"] = p.locked;
        t["points"].push_back(jsonP);
    }

    for(auto& s : sticks) {
        json jsonS;
        jsonS["indexA"] = s.indexA;
        jsonS["indexB"] = s.indexB;
        jsonS["lockLength"] = s.lockLength;
        t["sticks"].push_back(jsonS);
    }
    e["verletMesh"] = t;
}

/*
void VerletMeshComponent::buildRopeWithQuad(Vector2 translate) {
    points.emplace_back(VerletPoint(translate, true));

    for(u32 i = 1; i < 16; i++) {
        translate += Vector2(20,0);
        points.emplace_back(VerletPoint(translate, false));
    }


    for(u32 i = 0; i < points.size()-1; i++) {
        sticks.emplace_back(VerletStick(points[i], points[i+1]));
        order.emplace_back(i);
    }
}
*/
