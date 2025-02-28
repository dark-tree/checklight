#include "sceneManager.hpp"
#include "external.hpp"
#include "sceneInputListener.hpp"
#include "shared/math.hpp"
#include "render/system.hpp"


/*
 * SceneManager
 */

SceneManager::SceneManager() {
	currentScene = std::make_shared<Scene>();

	/*
	int consumable[] = { GLFW_KEY_W, GLFW_KEY_A,GLFW_KEY_S,GLFW_KEY_D,GLFW_KEY_Q,GLFW_KEY_E };
	for (int i = 0; i < sizeof(consumable) / sizeof(int); i++) {
		listener->toConsume[i] = true;
	}*/
}

void SceneManager::update() {
	currentScene->getMainCam().update();
}

std::shared_ptr<Scene> SceneManager::getMainScene() {
	return currentScene;
}