#include "models.hpp"

void Models::init(){
	initialized = true;
	RenderSystem& system = *RenderSystem::system;

	models = system.importObj("assets/models/default/shapes.obj");
}

std::shared_ptr<RenderModel> Models::getShape(Shape s){
	return models[(int)s];
}

void Models::terminate() {
	for(const auto& m : models){
		RenderSystem::system->closeModel(m); //this should be in destructor >:(
	}
	models.clear(); //:)
}
