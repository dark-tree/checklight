#include "source.hpp"

void SoundSourceObject::initSource(int number_of_sources){
	alGetError();
	this->number_of_sources = number_of_sources;
	this->sso_sources = new ALuint[this->number_of_sources];
	alGenSources(this->number_of_sources, sso_sources);
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR){
		throw std::runtime_error("Sound -> SSOinit: Failed to generate sources");  //throw exception
	}

	for (int i = 0;i < this->number_of_sources;i++)
	{
		alSourcef(this->sso_sources[i], AL_PITCH, 1.f);
		alSourcef(this->sso_sources[i], AL_GAIN, 1.f);
		alSource3f(this->sso_sources[i], AL_POSITION, 0,0,0);
		alSource3f(this->sso_sources[i], AL_VELOCITY, 0, 0, 0);
		alSourcei(this->sso_sources[i], AL_LOOPING, false);
	}
}

SoundSourceObject::SoundSourceObject(){
	SoundSourceObject::initSource(1);
}

SoundSourceObject::SoundSourceObject(int number_of_sources){
	SoundSourceObject::initSource(number_of_sources);
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

//@TODO reduce code lines, delete duplicates
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

void SoundSourceObject::addBuffer(std::shared_ptr<SoundClip> clip) {
	ALuint buffer = clip->getBuffer(0);
	if (buffer == 0) {

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

	alSourcePlay(this->sso_sources[0]);
	ALenum error = alGetError();
	if ((error = alGetError()) != AL_NO_ERROR) {
		throw std::runtime_error("Sound -> playSound: Failed to play clip");  //throw exception
	}

	/*ALint state;
	do {
		alGetSourcei(sso_sources[0], AL_SOURCE_STATE, &state);
	} while (state == AL_PLAYING);*/
}

bool SoundSourceObject::isPlaying(){
	ALint state;
	for (int i = 0;i < this->number_of_sources;i++){
		alGetSourcei(sso_sources[0], AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING) {
			return true;
		}
	}
	return false;
}