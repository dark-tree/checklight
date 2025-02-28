#pragma once
#include "scene.hpp"


Scene::Scene() : mainCam({18.0f,9.0f,4.0f}, {0.0f,0.0f,-1.0f,0.0f}, this) {
    id = idNumber++;
}

int Scene::getId() const {
    return id;
}

Camera& Scene::getMainCam() {
    return mainCam;
}

uint32_t Scene::idNumber = 0;