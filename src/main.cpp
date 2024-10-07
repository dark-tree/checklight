
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>

void glfwErrorCallback(int code, const char* description) {
	printf("[GLFW] %s\n", description);
}

void printExtensionCount() {
	uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	printf("[Vulkan] %d instance extensions supported\n", extensionCount);
}

int main() {

	// Notice how we can use Vulkan even before there
	// is a window open or GLFW has been initialized!
	printExtensionCount();

	glfwInit();
	glfwSetErrorCallback(glfwErrorCallback);

	if (!glfwVulkanSupported()) {
		printf("Vulkan is not supported! Uh oh!\n");
		return -1;
	}

	// We don't need no OpenGL!
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* glfw_window = glfwCreateWindow(200, 200, "Hello Vulkan", nullptr, nullptr);
	// no glfwMakeContextCurrent! That is a LibGL thing!

	while(!glfwWindowShouldClose(glfw_window)) {
        glfwPollEvents();

		// TODO: Silnik do gier w oparciu o bibliotekę Vulkan
    }

	glfwDestroyWindow(glfw_window);
	glfwTerminate();

}
