#include "source.hpp"

void SoundSourceObject::initSource(int number_of_sources){
	alGetError();
	this->number_of_sources = number_of_sources;
	this->sso_sources = new ALuint[this->number_of_sources];
	alGenSources(this->number_of_sources, sso_sources);
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR){
		std::cerr<<("Sound -> SSOinit: Failed to generate sources\f");  //throw exception
		return;
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

// SoundSourceObject constructor to create object with only one sound source

SoundSourceObject::SoundSourceObject(){
	SoundSourceObject::initSource(1);
}

// SoundSourceObject constructor to create a many sound sources in one object

//SoundSourceObject::SoundSourceObject(int number_of_sources){
//	SoundSourceObject::initSource(number_of_sources);
//}

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

	return 0;
}

void SoundSourceObject::setPosition(float x, float y, float z)
{
	alSource3f(this->sso_sources[0], AL_POSITION, x, y, z);
}


void SoundSourceObject::addBuffer(SoundClip clip){
	ALuint buffer = clip.getBuffer();
	if (buffer==0) {
		std::cerr << ("Source -> addBuffer: Buffer doesnt exist\f");	//throw exception
		return;
	}

	alSourcei(this->sso_sources[0], AL_BUFFER, buffer);

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << ("Source -> addBuffer: OpenAL error " + std::to_string(error) +"\f");
		return;
	}
}

void SoundSourceObject::addBuffer(std::shared_ptr<SoundClip> clip) {
	if (clip->getBuffer() == 0) {
		std::cerr << ("Source -> addBuffer: Buffer doesnt exist\f");	//throw exception
		return;
	}

	sc_buffer = clip;
	auto lockedClip = sc_buffer.lock();
	if (lockedClip){
		alSourcei(this->sso_sources[0], AL_BUFFER, lockedClip->getBuffer());
	}
	else {
		std::cerr << ("Source -> addBuffer: sc_buffer doesnt exist");
		return;
	}
	

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << ("Source -> addBuffer: OpenAL error " + std::to_string(error)+"\f");
		return;
	}
}


void SoundSourceObject::addGroupParameters(std::shared_ptr<SoundGroupParameters> sg) {
	this->sso_sg_parameters = sg;
	updateParameters();
}

void SoundSourceObject::updateParameters() {
	//if sso_sg_parameter has nullptr set default parameters for this SSO
	if (this->sso_sg_parameters == nullptr){
		for (int i = 0;i < this->number_of_sources;i++){
			alSourcef(this->sso_sources[i], AL_PITCH, 1.0f);
			alSourcef(this->sso_sources[i], AL_GAIN, 1.0f);
			alSourcei(this->sso_sources[i], AL_LOOPING, false);
		}
	}
	else {
		for (int i = 0;i < this->number_of_sources;i++) {
			alSourcef(this->sso_sources[i], AL_PITCH, sso_sg_parameters.get()->getPitch());
			alSourcef(this->sso_sources[i], AL_GAIN, sso_sg_parameters.get()->getGain());
			alSourcei(this->sso_sources[i], AL_LOOPING, sso_sg_parameters.get()->getLooping());
		}
	}
}

\
void SoundSourceObject::play(int source_number) {
	if (source_number > this->number_of_sources || !alIsSource(this->sso_sources[source_number])) {
		std::cerr << ("Source -> playSound: No valid sound source found!\f");
		return;
	}

	alSourcePlay(this->sso_sources[source_number]);
	ALenum error = alGetError();
	if ((error = alGetError()) != AL_NO_ERROR) {
		std::cerr << ("Sound -> playSound: Failed to play clip\f");  //throw exception
		return;
	}
}

void SoundSourceObject::playSound(){
	play(0);
}

void SoundSourceObject::stop(int source_number) {
	if (source_number > this->number_of_sources || !alIsSource(this->sso_sources[source_number])) {
		std::cerr << ("Source -> stopSound: No valid sound source found!\f");
		return;
	}

	alSourceStop(this->sso_sources[source_number]);
	ALenum error = alGetError();
	if ((error = alGetError()) != AL_NO_ERROR) {
		std::cerr << ("Sound -> stopSound: Failed to stop clip\f");  //throw exception
		return;
	}
}

void SoundSourceObject::stopSound(){
	stop(0);
}

void SoundSourceObject::pause(int source_number) {
	if (source_number > this->number_of_sources || !alIsSource(this->sso_sources[source_number])) {
		std::cerr << ("Source -> pauseSound: No valid sound source found!\f");
		return;
	}

	alSourcePause(this->sso_sources[source_number]);
	ALenum error = alGetError();
	if ((error = alGetError()) != AL_NO_ERROR) {
		std::cerr << ("Sound -> pauseSound: Failed to pause clip\f");  //throw exception
		return;
	}
}

void SoundSourceObject::pauseSound() {
	pause(0);
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