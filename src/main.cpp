
#include "render/system.hpp"
#include "render/model/importer.hpp"
#include "input/input.hpp"
#include "sound/sound.hpp"

SoundManager& sm = SoundManager::getInstance();

class tmpInput : public InputListener{
public:
	InputResult onEvent(const InputEvent& event) {
		if (const auto* nazwijtosobiejakosnpevent = event.as<KeyboardEvent>()) {
			if(nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_S)){
				sm.stopSound("s");
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_P)) {
				sm.playSound("s");
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_A)) {
				sm.pauseSound("s");
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_N)) {
				sm.playSound("sa");
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_M)) {
				sm.stopSound("sa");
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_B)) {
				sm.pauseSound("sa");
			}
		}


		return InputResult::PASS;
	}

	
	
};


int main() {

	ApplicationParameters parameters;
	parameters.setName("My Checklight Game");
	parameters.setDimensions(1200, 800);

	RenderSystem::init(parameters);

	RenderSystem& system = *RenderSystem::system;
	Window& window = system.getWindow();


	window.getInputDispatcher().registerListener(std::make_shared<tmpInput>());

	auto meshes = Importer::importObj(system, "assets/models/checklight.obj");

	//test sound
	//@TODO oddzielny watek dla sound managera tak wektor sourcow
	try {

		sm.createSource("s");
		sm.createClip("f");
		sm.addAudioToClip("f", "assets/sounds/testOGG.ogg");
		sm.connectClipWithSource("f", "s");

		sm.createSource("sa");
		sm.createClip("fa");
		sm.addAudioToClip("fa", "assets/sounds/test/2.ogg");
		sm.connectClipWithSource("fa", "sa");

		sm.playSound("s");

		SoundListener::setPosition(0, 10, 0);
		/*SoundClip sc;
		sc.addClip("assets/sounds/testOGG.ogg");
		SoundSourceObject sso;
		sso.setPosition(0, 1, 0);
		SoundListener::setPosition(0, 1, 0);
		sso.addBuffer(sc);
		sso.playSound();*/
	}
	catch (const std::runtime_error& e)
	{
		printf(e.what());
	}


	while (!window.shouldClose()) {
		window.poll();

		// update uniforms
		// do this once at the beginning of frame rendering
		system.setProjectionMatrix(40.0f, 0.1f, 1000.0f);
		system.setViewMatrix({18.0f, 1.0f, 4.0f}, {-21.0f, 0.0f, 4.0f});
		system.updateUniforms();

		// all rendering must be done between beginDraw() and endDraw()
		system.beginDraw();

		// this does nothing for now, this operation will most likely be expensive later
		system.bindMaterial();

		for (auto& mesh : meshes) {
			glm::mat4 model = glm::identity<glm::mat4>();

			// first bind the mesh
			system.bindMesh(mesh);

			// then you can draw it multiple times
			system.draw(model);

			// later using a precomputed position vector may be reqired by the renderer
			// take that into account
		}

		// each frame must end with this call or the engine will deadlock
		system.endDraw();
    }

	system.wait();

	for (auto& mesh : meshes) {
		mesh.reset();
	}

	RenderSystem::system.reset();

}
