#pragma once
#include "camera.hpp"
#include "external.hpp"


class Scene {
protected:
    static uint32_t idNumber;
    uint32_t id;
    Camera mainCam;
public:
    Scene();

    int getId() const;

    Camera& getMainCam();
};