#include "source.hpp"

SoundSourceObject::SoundSourceObject(){
	alGetError();
	this->number_of_sources = 1;
	this->sso_sources = new ALuint[this->number_of_sources];
	alGenSources(this->number_of_sources, sso_sources);
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}
}

SoundSourceObject::SoundSourceObject(int numberOfSources){
	alGetError();
	this->number_of_sources = numberOfSources;
	this->sso_sources = new ALuint[this->number_of_sources];
	alGenSources(this->number_of_sources, sso_sources);
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR){
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}
}

SoundSourceObject::~SoundSourceObject(){
	alDeleteSources(this->number_of_sources,this->sso_sources);
}


ALuint SoundSourceObject::getSource(){
	if (alIsSource(sso_sources[0]) == AL_TRUE){
		return sso_sources[0];
	}

	return 0;
}

ALuint SoundSourceObject::getSource(int number){
	if (alIsSource(sso_sources[number]) == AL_TRUE){
		return sso_sources[number];
	}

	return NULL;
}

void SoundSourceObject::setPosition(float x, float y, float z)
{
	alSource3f(this->sso_sources[0], AL_POSITION, x, y, z);
}
//@TODO check if exist


void SoundSourceObject::addBuffer(SoundClip clip){
	ALuint buffer = clip.getBuffer(0);
	if (buffer==0) {

		throw std::runtime_error("Source -> addBuffer: Buffer doesnt exist");	//throw exception
	}

	alSourcei(this->sso_sources[0], AL_BUFFER, buffer);

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw std::runtime_error("Source -> addBuffer: OpenAL error " + std::to_string(error));
	}
}

void SoundSourceObject::addBuffers(SoundClip buffer){

}

void SoundSourceObject::playSound()
{
	if (!alIsSource(this->sso_sources[0])) {
		throw std::runtime_error("Source -> playSound: No valid sound source found!");
	}

	alSourcef(this->sso_sources[0], AL_GAIN, 1.0f);
	alSourcef(this->sso_sources[0], AL_PITCH, 1.0f);
	alSourcePlay(this->sso_sources[0]);
}