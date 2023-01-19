//
// Created by bison on 08-01-23.
//

#ifndef PLATFORMER_VERLETSYSTEM_H
#define PLATFORMER_VERLETSYSTEM_H

#include <ECS.h>
#include "../components/VerletMeshComponent.h"

using namespace ECS;
using namespace Renderer;

class VerletSystem : public EntitySystem  {
public:
    explicit VerletSystem() = default;
    ~VerletSystem() override = default;
    void tick(World* world, float deltaTime) override;

private:
    void updateVerletMesh(VerletMeshComponent& verlet, float deltaTime);
};


#endif //PLATFORMER_VERLETSYSTEM_H
