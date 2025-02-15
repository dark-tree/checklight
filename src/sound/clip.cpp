#include "clip.hpp"

SoundClip::SoundClip()
{
	alGetError();
	alGenBuffers(1, SCbuffers);
	this->numberOfBuffers = 1;
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Clip -> SCinit: Failed to generate buffers");  //throw exception
	}
}

SoundClip::SoundClip(int numberOfBuf)
{
	alGetError();
	alGenBuffers(numberOfBuf, SCbuffers);
	this->numberOfBuffers = numberOfBuf;
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Clip -> SCinit: Failed to generate buffers");  //throw exception
	}
}

SoundClip::~SoundClip()
{
	alDeleteBuffers(this->numberOfBuffers, this->SCbuffers);
}