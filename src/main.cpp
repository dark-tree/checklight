#include "render/system.hpp"
#include "render/model/importer.hpp"
#include "input/input.hpp"
#include "sound/sound.hpp"

SoundManager& sm = SoundManager::getInstance();
auto sso1 = std::make_shared<SoundSourceObject>();
auto sso2 = std::make_shared<SoundSourceObject>();
auto sg = std::make_shared<SoundGroup>();
SoundListener listener;

class tmpInput : public InputListener{
public:
	InputResult onEvent(const InputEvent& event) {
		if (const auto* nazwijtosobiejakosnpevent = event.as<KeyboardEvent>()) {
			if(nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_S)){
				sm.stopSound(sso1);
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_P)) {
				sm.playSound(sso1);
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_A)) {
				sm.pauseSound(sso1);
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_N)) {
				sm.playSound(sso2);
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_M)) {
				sm.stopSound(sso2);
			}else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_B)) {
				sm.pauseSound(sso2);
			}
			else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_H)) {
				sg->setMute(true);
			}
			else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_J)) {
				sg->setMute(false);
			}
			//NIE WYCZUWALNA ROZNICA DO NAPRAWY 
			else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_UP)) {
				listener.setPosition(1000, 10, 0);
				listener.print();
				//sso1->print();

				//sso1->setPosition(900000.0f,0.0f,5.0f);
			}
			else if (nazwijtosobiejakosnpevent->wasPressed(GLFW_KEY_DOWN)) {
				listener.setPosition(0, 00, 0);
				//sso1->setPosition(0.0f,0.0f,0.0f);
				//listener.print();
				//sso1->print();
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
		
		auto sc1 = std::make_shared<SoundClip>();
		auto sc2 = std::make_shared<SoundClip>();
		

		sm.addSource(sso1);
		sm.addClip(sc1);
		sm.addAudioToClip(sc1, "assets/sounds/testOGG.ogg");
		sm.connectClipWithSource(sc1, sso1);

		sm.addSource(sso2);
		sm.addClip(sc2);
		sm.addAudioToClip(sc2, "assets/sounds/test/2.ogg");
		sm.connectClipWithSource(sc2, sso2);

		//sm.playSound(sso1);
		listener.setDistanceModel(AL_INVERSE_DISTANCE);
		listener.setPosition(1000.0f, 10.0f, 1000.0f);

		
		//sso1->addGroupParameters(sg);
		//sg->addObserversSoundSourceObject(sso1);
		//sg->setPitch(0.5f);
		sso1->setGain(0.5f);
		sso1->setPitch(0.5f);
		sso1->setMaxDistance(100.0f);
		sso1->setReferenceDistance(1.0f);
		sso1->setRolloffFactor(1.0f);
		sso1->setMinGain(0.0f);

		//sg->setMaxDistance(0.0f);
		//sg->setReferenceDistance(0.0f);
		//sg->setRolloffFactor(0.0f);
		//sg->setMinGain(0.0f);
		//sso1->setMute(true);

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
