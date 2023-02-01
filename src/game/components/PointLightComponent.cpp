//
// Created by bison on 06-01-23.
//

#include "PointLightComponent.h"
#include "../PolyUtil.h"

PointLightComponent::PointLightComponent(float innerRadius, float outerRadius, const Color &innerColor,
                               const Color &middleColor, const Color &outerColor, uint32_t segments)
        : innerRadius(innerRadius), outerRadius(outerRadius), innerColor(innerColor),
          middleColor(middleColor), outerColor(outerColor), segments(segments) {
    rebuildMesh();
}

PointLightComponent::PointLightComponent(const json &e) {
    innerRadius = e["innerRadius"];
    outerRadius = e["outerRadius"];
    auto color = e["innerColor"];
    innerColor = { color[0], color[1], color[2], color[3] };
    color = e["middleColor"];
    middleColor = { color[0], color[1], color[2], color[3] };
    color = e["outerColor"];
    outerColor = { color[0], color[1], color[2], color[3] };
    segments = e["segments"];
    if(e.contains("posOffset")) {
        posOffset = Vector2(e["posOffset"][0], e["posOffset"][1]);
    }
    rebuildMesh();
}

void PointLightComponent::rebuildMesh() {
    innerMesh.clear();
    outerMesh.clear();
    buildTorusTriangleStripMesh(Vector2(0,0), outerRadius, innerRadius, segments, outerColor, middleColor, outerMesh);
    buildTorusTriangleStripMesh(Vector2(0,0), innerRadius, 0, segments, middleColor, innerColor, innerMesh);
}


void PointLightComponent::save(json &e) {
    json j;
    j["innerRadius"] = innerRadius;
    j["outerRadius"] = outerRadius;
    pushColor(j["innerColor"], innerColor);
    pushColor(j["middleColor"], middleColor);
    pushColor(j["outerColor"], outerColor);
    j["segments"] = segments;
    pushPos(j["posOffset"], posOffset);
    e["pointLight"] = j;
}

