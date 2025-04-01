#pragma once

#ifndef SOUND_LISTENER_HPP
#define SOUND_LISTENER_HPP
#include "external.hpp"

class SoundListener {
public:
	static void setPosition(float x, float y, float z){
		alListener3f(AL_POSITION, x, y, z);
	}

	static void setVelocity(float x, float y, float z){
		alListener3f(AL_VELOCITY, x, y, z);
	}

	static void setDistanceModel(ALenum distance_model) {
		alDistanceModel(distance_model);
	}

	float* getPosition() {
		float* position = new float[3];
		alGetListenerfv(AL_POSITION, position);
		return position;
	}

	float* getVelocity() {
		float* velocity = new float[3];
		alGetListenerfv(AL_VELOCITY,velocity);
		return velocity;
	}

	ALenum getDistanceModel() {
		ALint model;
		alGetIntegerv(AL_DISTANCE_MODEL, &model);
		return model;
	}

	void print() {
		auto position = getPosition();
		auto velocity = getVelocity();
		ALenum distanceModel = getDistanceModel();

		std::cout << "SoundListener:" << std::endl;
		std::cout << "  Position: (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
		std::cout << "  Velocity: (" << velocity[0] << ", " << velocity[1] << ", " << velocity[2] << ")" << std::endl;
		std::cout << "  Distance Model: " << distanceModel << std::endl;
	}
};

#endif //SOUND_LISTENER_HPP
