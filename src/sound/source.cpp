#include "source.hpp"

SoundSourceObject::SoundSourceObject()
{
	alGetError();
	alGenSources(1, sso_sources);
	this->number_of_sources = 1;
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}
}

SoundSourceObject::SoundSourceObject(int numberOfSources)
{
	alGetError();
	alGenSources(numberOfSources, sso_sources);
	this->number_of_sources = numberOfSources;
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}
}

SoundSourceObject::~SoundSourceObject()
{
	alDeleteSources(this->number_of_sources,this->sso_sources);
}


ALuint SoundSourceObject::getSource()
{
	if (alIsSource(sso_sources[0]) == AL_TRUE)
	{
		return sso_sources[0];
	}

	return 0;
}

ALuint SoundSourceObject::getSource(int number)
{
	if (alIsSource(sso_sources[number]) == AL_TRUE)
	{
		return sso_sources[number];
	}

	return 0;
}

void SoundSourceObject::addBuffer(SoundClip buffer)
{

}

void SoundSourceObject::addBuffers(SoundClip buffer)
{

}