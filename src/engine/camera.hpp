#pragma once
#include "spatialObject.hpp"
#include "external.hpp"
#include "sceneInputListener.hpp"

const float MOUSE_SENSITIVITY = 400.0;
const bool IS_VERTICALLY_LOCKED = false;

class Scene;

class Camera : public SpatialObject {
private:
    Scene* s;
    //TEMPORARY!!!!!!!!!!!!!!!!!!!!!!!! usunac scene* s

    std::shared_ptr<SceneInputListener> sceneInputListener;
    glm::vec2 currentMousePos;
    glm::vec2 prevMousePos;
    glm::vec2 posDifference;
    float DELTATIME;
    bool init;
public:
	Camera(Scene* s);

	Camera(glm::vec3 pos, Scene* s);
	Camera(glm::vec3 pos, glm::quat rot, Scene* s);

	void update() override;
};