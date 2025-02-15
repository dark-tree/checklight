#include "source.hpp"

SoundSourceObject::SoundSourceObject()
{
	alGetError();
	alGenSources(1, SSOsources);
	this->numberOfSources = 1;
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}
}

SoundSourceObject::SoundSourceObject(int numberOfSources)
{
	alGetError();
	alGenSources(numberOfSources, SSOsources);
	this->numberOfSources = numberOfSources;
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}
}

SoundSourceObject::~SoundSourceObject()
{
	alDeleteSources(this->numberOfSources,this->SSOsources);
}


ALuint SoundSourceObject::getSource()
{
	if (alIsSource(SSOsources[0]) == AL_TRUE)
	{
		return SSOsources[0];
	}

	return 0;
}

ALuint SoundSourceObject::getSource(int number)
{
	if (alIsSource(SSOsources[number]) == AL_TRUE)
	{
		return SSOsources[number];
	}

	return 0;
}