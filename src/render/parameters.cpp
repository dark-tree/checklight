
#include "parameters.hpp"
#include "frame.hpp"

/*
 * RenderParameters
 */

void RenderParameters::updateSceneUniform(SceneUniform& uniform) const {
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

void RenderParameters::setAmbientLight(glm::vec3 color) {
	this->ambient_color = color;
}
