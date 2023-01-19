//
// Created by bison on 10-01-23.
//

#ifndef PLATFORMER_PATHCOMPONENT_H
#define PLATFORMER_PATHCOMPONENT_H

#include "../JsonUtil.h"

struct PathComponent {
    PathComponent(std::vector<Vector2> waypoints, bool reverse) : waypoints(std::move(waypoints)),
                                                                  reverse(reverse) {}

    PathComponent(const PathComponent& old) {
        waypoints = old.waypoints;
        reverse = old.reverse;
        nextWaypoint = 1;
        direction = 1;
    }

    explicit PathComponent(const json& e) {
        auto waypoints = e["waypoints"];
        for(i32 i = 0; i < waypoints.size(); i += 2) {
            Vector2 vertex(waypoints[i], waypoints[i+1]);
            this->waypoints.emplace_back(vertex);
        }
        reverse = e["reverse"];
    }

    std::vector<Vector2> waypoints;

    bool reverse = false;
    i32 nextWaypoint = 1;
    i32 direction = 1;

    void next() {
        nextWaypoint += direction;
        if(nextWaypoint > waypoints.size()-1 && direction == 1) {
            if(reverse) {
                direction = -1;
                nextWaypoint -= 2;
            } else {
                nextWaypoint = 0;
            }
        } else if(nextWaypoint < 0 && direction == -1) {
            direction = 1;
            nextWaypoint = 1;
        }
    }

    bool onFirst() {
        return nextWaypoint == 0;
    }
    bool onLast() {
        return nextWaypoint == waypoints.size()-1;
    }

    Vector2& fromWaypoint() {
        i32 i = nextWaypoint;
        if(direction == 1)
            i--;
        if(direction == -1)
            i++;

        if(i < 0) {
            i = (i32) waypoints.size()-1;
        } else if(i > waypoints.size()-1) {
            i = 0;
        }
        return waypoints[i];
    }

    Vector2& toWaypoint() {
        return waypoints[nextWaypoint];
    }

    void save(json& e) {
        json j;
        j["reverse"] = reverse;
        json points;
        for(auto& pos : waypoints) {
            pushPos(points, pos);
        }
        j["waypoints"] = points;
        e["path"] = j;
    }
};

#endif //PLATFORMER_PATHCOMPONENT_H
