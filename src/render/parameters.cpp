
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
	uniform.gi_brightness = gi_brightness;
	uniform.portal_tint = portal_tint;
	uniform.portal_jumps = portal_jumps;
	uniform.portal_gi = portal_gi;
	uniform.sky_color = sky_color;
	uniform.sky_emission = sky_emission;
	uniform.gi_max_sample_age = gi_max_sample_age;
}

int RenderParameters::getGISamples() const {
	return gi_samples;
}

void RenderParameters::setGISamples(int samples) {
	this->gi_samples = samples;
}

float RenderParameters::getGIBrightness() const {
	return gi_brightness;
}

void RenderParameters::setGIBrightness(float brightness) {
	this->gi_brightness = brightness;
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

glm::vec3 RenderParameters::getAmbientLight() const {
	return ambient_color;
}

void RenderParameters::setAmbientLight(glm::vec3 color) {
	this->ambient_color = color;
}

glm::vec3 RenderParameters::getPortalTint() const {
	return portal_tint;
}

void RenderParameters::setPortalTint(glm::vec3 color) {
	this->portal_tint = color;
}

int RenderParameters::getPortalMaxJumps() const {
	return portal_jumps;
}

void RenderParameters::setPortalMaxJumps(int jumps) {
	this->portal_jumps = jumps;
}

bool RenderParameters::getPortalGIEnable() const {
	return portal_gi;
}

void RenderParameters::setPortalGIEnable(bool gi) {
	this->portal_gi = gi;
}

glm::vec3 RenderParameters::getSkyColor() const {
	return sky_color;
}

void RenderParameters::setSkyColor(glm::vec3 color) {
	this->sky_color = color;
}

float RenderParameters::getSkyEmission() const {
	return sky_emission;
}

void RenderParameters::setSkyEmission(float emission) {
	this->sky_emission = emission;
}

int RenderParameters::getMaxSampleAge() const {
	return gi_max_sample_age;
}

void RenderParameters::setMaxSampleAge(int age) {
	this->gi_max_sample_age = age;
}
