#pragma once

#ifndef SOUND_LISTENER_HPP
#define SOUND_LISTENER_HPP
#include "external.hpp"

/// Class to manage the sound listener in OpenAL.
class SoundListener {
public:
	static void setPosition(float x, float y, float z){
		alListener3f(AL_POSITION, x, y, z);
	}

	static void setPosition(glm::vec3 position) {
		alListener3f(AL_POSITION, position.x, position.y, position.z);
	}

	static void setVelocity(float x, float y, float z){
		alListener3f(AL_VELOCITY, x, y, z);
	}

	static void setVelocity(glm::vec3 velocity) {
		alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	}

	static void setOrientation(float x,float y, float z) {
		alListener3f(AL_ORIENTATION, z, y, z);
	}


	/// Set the listener's orientation using two vectors: the "at" vector and the "up" vector.
	///
	/// @param at The "at" vector, representing the direction the listener is facing.
	/// @param up The "up" vector, representing the upward direction relative to the listener.
	static void setOrientation(glm::vec3 at, glm::vec3 up) {
		float orientation[] = { at.x, at.y, at.z, up.x, up.y, up.z };
		alListenerfv(AL_ORIENTATION, orientation);
	}

	static void setDistanceModel(ALenum distance_model) {
		alDistanceModel(distance_model);
	}

	static float* getPosition() {
		float* position = new float[3];
		alGetListenerfv(AL_POSITION, position);
		return position;
	}

	static glm::vec3 getPositionV3() {
		glm::vec3 position;
		alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);
		return position;
	}

	static float* getVelocity() {
		float* velocity = new float[3];
		alGetListenerfv(AL_VELOCITY,velocity);
		return velocity;
	}

	static glm::vec3 getVelocityV3() {
		glm::vec3 velocity;
		alGetListener3f(AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);
		return velocity;
	}

	/// Get the listener's orientation as two vectors: the "at" vector and the "up" vector.
	/// The returned array contains 6 floats: [at_x, at_y, at_z, up_x, up_y, up_z].
	static float* getOrientation() {
		float* orientation = new float[6];
		alGetListenerfv(AL_ORIENTATION, orientation);
		return orientation;
	}

	static ALenum getDistanceModel() {
		ALint model;
		alGetIntegerv(AL_DISTANCE_MODEL, &model);
		return model;
	}

	static void print() {
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
