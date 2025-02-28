#include "camera.hpp"
#include "render/system.hpp"
#include "scene.hpp"
#include "sceneInputListener.hpp"
#include "sceneManager.hpp"
#include "shared/math.hpp"
#include "scene.hpp"

/*
 * Camera
 */

Camera::Camera(Scene* ns) : SpatialObject() {
	sceneInputListener = std::make_shared<SceneInputListener>();
	RenderSystem::system->getWindow().getInputDispatcher().registerListener(sceneInputListener);

	s = ns;

	init = false;
	DELTATIME = 0.02;
};

Camera::Camera(glm::vec3 pos, Scene* ns) : SpatialObject(pos) {
	sceneInputListener = std::make_shared<SceneInputListener>();
	RenderSystem::system->getWindow().getInputDispatcher().registerListener(sceneInputListener);

	s = ns;
	init = false;
	DELTATIME = 0.02;
};

Camera::Camera(glm::vec3 pos, glm::quat rot, Scene* ns) : SpatialObject(pos, rot) {
	sceneInputListener = std::make_shared<SceneInputListener>();
	RenderSystem::system->getWindow().getInputDispatcher().registerListener(sceneInputListener);

	s = ns;
	init = false;
	DELTATIME = 0.02;
};

void Camera::update() {

	if (init == false) {
		prevMousePos = sceneInputListener->mousePos;
		init = true;
	}

	currentMousePos = sceneInputListener->mousePos;
	posDifference = prevMousePos - currentMousePos;
	prevMousePos = currentMousePos;

	//2 pi rad
	float rotationXrad = posDifference.x / MOUSE_SENSITIVITY;
	float rotationYrad = posDifference.y / MOUSE_SENSITIVITY;

	Camera& c = s->getMainCam();
	c.setRotation(glm::rotate(c.getRotation(), rotationYrad, glm::vec3(0, 0, 1)));
	c.setRotation(glm::rotate(c.getRotation(), rotationXrad, glm::vec3(0, 1, 0)));

	//glm::vec3 eulerCameraRotation = glm::eulerAngles(c.getRotation());
	//float cameraRotationY = eulerCameraRotation.y;

	glm::vec3 forward = math::calculateForwardVector(c.getRotation());

	glm::vec3 plain;
	if (IS_VERTICALLY_LOCKED) {
		plain = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	}
	else {
		plain = glm::normalize(forward);
	}

	glm::vec3 plainPerpendicular = glm::cross(plain, glm::vec3(0, 1, 0));

	float cameraRotationY = atan2(plain.y, plain.x);

	/*if (cameraRotationY < 0) {
		cameraRotationY += 2
	}*/

	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_W] == true) {
		c.setPosition(c.getPosition() + glm::vec3{
			plain.x * DELTATIME,
			plain.y * DELTATIME,
			plain.z * DELTATIME
			});
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_A] == true) {
		c.setPosition(c.getPosition() + glm::vec3{
			-plainPerpendicular.x * DELTATIME,
			0,
			-plainPerpendicular.z * DELTATIME
			});
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_S] == true) {
		c.setPosition(c.getPosition() + glm::vec3{
			-plain.x * DELTATIME,
			-plain.y * DELTATIME,
			-plain.z * DELTATIME
			});
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_D] == true) {
		c.setPosition(c.getPosition() + glm::vec3{
			plainPerpendicular.x * DELTATIME,
			0,
			plainPerpendicular.z * DELTATIME
			});
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_LEFT_SHIFT] == true) {
		c.setPosition(c.getPosition() + glm::vec3{ 0, -1 * DELTATIME, 0 });
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_SPACE] == true) {
		c.setPosition(c.getPosition() + glm::vec3{ 0, 1 * DELTATIME, 0 });
	}

	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_1] == true) {
		DELTATIME = 0.005f;
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_2] == true) {
		DELTATIME = 0.01f;
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_3] == true) {
		DELTATIME = 0.02f;
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_4] == true) {
		DELTATIME = 0.04f;
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_5] == true) {
		DELTATIME = 0.08f;
	}

	if (sceneInputListener->LMBPressed == true) {
		RenderSystem::system->getWindow().setMouseCapture(true);
	}
	if (sceneInputListener->sceneCurrentlyPressed[GLFW_KEY_ESCAPE] == true) {
		RenderSystem::system->getWindow().setMouseCapture(false);
	}

	//eulerCameraRotation.x -= rotationXrad;
	//eulerCameraRotation.y -= rotationYrad;

	//eulerCameraRotation.x = glm::clamp(eulerCameraRotation.x, -glm::half_pi<float>(), glm::half_pi<float>());

	/*
	glm::quat quatX = glm::angleAxis(eulerCameraRotation.x, glm::vec3(1.0, 0.0, 0.0));
	glm::quat quatY = glm::angleAxis(eulerCameraRotation.y, glm::vec3(0.0, 1.0, 0.0));
	glm::quat quatZ = glm::angleAxis(eulerCameraRotation.z, glm::vec3(0.0, 0.0, 1.0));
	glm::quat finalQ = quatX * quatY * quatZ;*/

	//c.setRotation(glm::normalize(glm::quat(eulerCameraRotation)));
	//glm::rotate
}