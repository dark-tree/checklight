
#include "parameters.hpp"
#include "frame.hpp"

/*
 * RenderParameters
 */

void RenderParameters::updateSceneUniform(SceneUniform& uniform) const {

	uniform.dir_light_direction = dir_light_direction;
	uniform.dir_light_color = dir_light_color;
	uniform.ambient_color = ambient_color;

	uniform.gi_samples = gi_samples;
	uniform.denoise = denoise;
	uniform.shadows = shadows;
}

int RenderParameters::getGISamples() const {
	return gi_samples;
}

void RenderParameters::setGISamples(int samples) {
	gi_samples = samples;
}

bool RenderParameters::getDenoise() const {
	return denoise;
}

void RenderParameters::setDenoise(bool denoise) {
	this->denoise = denoise;
}

bool RenderParameters::getShadows() const {
	return shadows;
}

void RenderParameters::setShadows(bool shadows) {
	this->shadows = shadows;
}

void RenderParameters::setDirectionalLight(glm::vec3 direction, glm::vec3 color) {
	this->dir_light_direction = direction;
	this->dir_light_color = color;
}

void RenderParameters::setAmbientLight(glm::vec3 color) {
	this->ambient_color = color;
}
