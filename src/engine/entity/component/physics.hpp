#pragma once
#include "game.hpp"
#include "render/render.hpp"
#include "engine/data/models.hpp"

#pragma once

class PhysicsComponent : public GameComponent{
protected:
    std::shared_ptr<RenderObject> renderObject;
public:
    PhysicsComponent(SpatialPawn* sp, Models::Shape s);

    ~PhysicsComponent() override;
protected:

};

