#pragma once
#include "external.hpp"
#include "scene.hpp"

class SceneInputListener;

class SceneManager {
private:
    std::shared_ptr<Scene> currentScene;

public:
    SceneManager();

    void update();

    std::shared_ptr<Scene> getMainScene();
};
