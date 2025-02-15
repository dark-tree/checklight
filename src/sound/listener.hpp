#pragma once

#ifndef SOUND_LISTENER_HPP
#define SOUND_LISTENER_HPP
#include "external.hpp"

class SoundListener {
public:
	static void setPosition(float x, float y, float z)
	{
		alListener3f(AL_POSITION, x, y, z);
	}

	static void setVelocity(float x, float y, float z)
	{
		alListener3f(AL_VELOCITY, x, y, z);
	}

	/*static std::array<float,3> getPosition()
	{
		std::array<float,3> positions;
		alGetListenerfv(AL_POSITION, positions.data);
		return positions;
	}*/
};

#endif //SOUND_LISTENER_HPP
