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

	updateParameters();
	updateMovement();
}

// SoundSourceObject constructor to create object with only one sound source

SoundSourceObject::SoundSourceObject(){
	SoundSourceObject::initSource(1);
}


SoundSourceObject::~SoundSourceObject(){
	alDeleteSources(this->number_of_sources,this->sso_sources);
	delete[] sso_sources;
	sso_sources = nullptr;
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


//========================MOVEMENT========================

void SoundSourceObject::setPosition(float x, float y, float z)
{
	sso_position = { x,y,z };
	updateMovement();
}

void SoundSourceObject::setVelocity(float x, float y, float z)
{
	sso_velocity = { x,y,z };
	updateMovement();
}

void SoundSourceObject::setDirection(float x, float y, float z)
{
	sso_direction = { x,y,z };
	updateMovement();
}

glm::vec3 SoundSourceObject::getRealPosition() {
	if (sso_sg != nullptr) {
		return sso_position + sso_sg->getPosition();
	}
	return sso_position;
}

glm::vec3 SoundSourceObject::getRealVelocity() {
	if (sso_sg != nullptr) {
		return sso_velocity + sso_sg->getVelocity();
	}
	return sso_velocity;
}

glm::vec3 SoundSourceObject::getRealDirection() {
	if (sso_sg != nullptr) {
		return sso_direction + sso_sg->getDirection();
	}
	return sso_direction;
}

//========================PARAMETERS========================


void SoundSourceObject::setMute(bool mute) {
	sso_is_mute = mute;
	updateParameters();
}

void SoundSourceObject::setPitch(float pitch) {
	sso_pitch = pitch;
	updateParameters();
}

void SoundSourceObject::setGain(float gain) {
	sso_gain = gain;
	updateParameters();
}

void SoundSourceObject::setLooping(bool looping) {
	sso_looping = looping;
	updateParameters();
}

void SoundSourceObject::setMaxDistance(float max_distance) {
	sso_max_distance = max_distance;
	updateParameters();
}

void SoundSourceObject::setMinGain(float min_gain) {
	sso_min_gain = min_gain;
	updateParameters();
}

void SoundSourceObject::setMaxGain(float max_gain) {
	sso_max_gain = max_gain;
	updateParameters();
}

void SoundSourceObject::setRolloffFactor(float rolloff_factor) {
	sso_rolloff_factor = rolloff_factor;
	updateParameters();
}

void SoundSourceObject::setReferenceDistance(float reference_distance) {
	sso_reference_distance = reference_distance;
	updateParameters();
}

void SoundSourceObject::setConeOuterGain(float cone_outer_gain) {
	sso_cone_outer_gain = cone_outer_gain;
	updateParameters();
}

void SoundSourceObject::setConeInnerAngle(float cone_inner_angle) {
	sso_cone_inner_angle = cone_inner_angle;
	updateParameters();
}

void SoundSourceObject::setConeOuterAngle(float cone_outer_angle) {
	sso_cone_outer_angle = cone_outer_angle;
	updateParameters();
}


//========================BUFFER========================


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


//========================SOUND GROUP========================


void SoundSourceObject::addGroupParameters(std::shared_ptr<SoundGroup> sg) {
	this->sso_sg = sg;
	updateParameters();
	updateMovement();
}


void SoundSourceObject::updateParameters() {
	//if sso_sg_parameter has nullptr set default parameters for this SSO
	if (this->sso_sg != nullptr){
		for (int i = 0;i < this->number_of_sources;i++) {
			alSourcef(this->sso_sources[i], AL_PITCH, sso_sg->getPitch() + sso_pitch);

			if (sso_sg->getMute() || sso_is_mute){
				alSourcef(this->sso_sources[i], AL_GAIN,0);
			}
			else
				alSourcef(this->sso_sources[i], AL_GAIN, sso_sg->getGain() + sso_gain);
			alSourcei(this->sso_sources[i], AL_LOOPING, sso_sg->getLooping() || sso_looping);

			float max_gain = (sso_sg->getMaxGain() > sso_max_distance) ? sso_sg->getMaxGain() : sso_max_gain;
			float min_gain = (sso_sg->getMinGain() <= sso_max_distance) ? sso_sg->getMinGain() : sso_min_gain;

			if (min_gain > max_gain)
				std::cerr << ("Source -> updateParameters: min_gain is bigger than max_gain\f");
				
			alSourcef(this->sso_sources[i], AL_MAX_DISTANCE, max_gain);
			alSourcef(this->sso_sources[i], AL_MAX_DISTANCE, min_gain);

			//@TODO zastanowic sie nad tymi parametrami
			alSourcef(this->sso_sources[i], AL_ROLLOFF_FACTOR, sso_sg->getRolloffFactor() + sso_rolloff_factor);
			alSourcef(this->sso_sources[i], AL_REFERENCE_DISTANCE, sso_sg->getReferenceDistance() + sso_reference_distance);
			alSourcef(this->sso_sources[i], AL_CONE_OUTER_GAIN, sso_sg->getConeOuterGain() + sso_cone_outer_gain);
			alSourcef(this->sso_sources[i], AL_CONE_INNER_ANGLE, sso_sg->getConeInnerAngle() + sso_cone_inner_angle);
			alSourcef(this->sso_sources[i], AL_CONE_OUTER_ANGLE, sso_sg->getConeOuterAngle() + sso_cone_outer_angle);
		}
	}
	else {
		for (int i = 0;i < this->number_of_sources;i++) {
			alSourcef(this->sso_sources[i], AL_PITCH, sso_pitch);
			alSourcef(this->sso_sources[i], AL_GAIN, sso_gain);
			alSourcei(this->sso_sources[i], AL_LOOPING, sso_looping);
			alSourcef(this->sso_sources[i], AL_MAX_DISTANCE, sso_max_distance);
			alSourcef(this->sso_sources[i], AL_MIN_GAIN, sso_min_gain);
			alSourcef(this->sso_sources[i], AL_MAX_GAIN, sso_max_gain);
			alSourcef(this->sso_sources[i], AL_ROLLOFF_FACTOR, sso_rolloff_factor);
			alSourcef(this->sso_sources[i], AL_REFERENCE_DISTANCE, sso_reference_distance);
			alSourcef(this->sso_sources[i], AL_CONE_OUTER_GAIN, sso_cone_outer_gain);
			alSourcef(this->sso_sources[i], AL_CONE_INNER_ANGLE, sso_cone_inner_angle);
			alSourcef(this->sso_sources[i], AL_CONE_OUTER_ANGLE, sso_cone_outer_angle);
		}
	}
}


void SoundSourceObject::updateMovement() {
	if (this->sso_sg != nullptr) {
		for (int i = 0;i < this->number_of_sources;i++) {
			glm::vec3 actual_position = { 0.0f,0.0f,0.0f };
			actual_position.x= sso_position.x + sso_sg->getPosition().x;
			actual_position.y = sso_position.y + sso_sg->getPosition().y;
			actual_position.z = sso_position.z + sso_sg->getPosition().z;
			alSource3f(this->sso_sources[i], AL_POSITION, actual_position.x, actual_position.y, actual_position.z);

			glm::vec3 actual_velocity = { 0.0f,0.0f,0.0f };
			actual_velocity.x = sso_velocity.x + sso_sg->getVelocity().x;
			actual_velocity.y = sso_velocity.y + sso_sg->getVelocity().y;
			actual_velocity.z = sso_velocity.z + sso_sg->getVelocity().z;

			alSource3f(this->sso_sources[i], AL_VELOCITY, actual_velocity.x, actual_velocity.y, actual_velocity.z);
			
			glm::vec3 actual_direction = { 0.0f,0.0f,0.0f };
			actual_direction.x = sso_direction.x + sso_sg->getDirection().x;
			actual_direction.y = sso_direction.y + sso_sg->getDirection().y;
			actual_direction.z = sso_direction.z + sso_sg->getDirection().z;
			alSource3f(this->sso_sources[i], AL_DIRECTION, actual_direction.x, actual_direction.y, actual_direction.z);
		}
	}
	else {
		for (int i = 0;i < this->number_of_sources;i++) {
			alSource3f(this->sso_sources[i], AL_POSITION, sso_position.x, sso_position.y, sso_position.z);
			alSource3f(this->sso_sources[i], AL_VELOCITY, sso_velocity.x, sso_velocity.y, sso_velocity.z);
			alSource3f(this->sso_sources[i], AL_DIRECTION, sso_direction.x, sso_direction.y, sso_direction.z);
		}
	}
}


//========================CONTROLLS========================


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

void SoundSourceObject::print() const {
	ALfloat pitch, gain, max_distance, min_gain, max_gain, rolloff_factor, reference_distance, cone_outer_gain, cone_inner_angle, cone_outer_angle;
	ALint looping;
	ALfloat position[3], velocity[3], direction[3];

	alGetSourcef(sso_sources[0], AL_PITCH, &pitch);
	alGetSourcef(sso_sources[0], AL_GAIN, &gain);
	alGetSourcei(sso_sources[0], AL_LOOPING, &looping);
	alGetSourcef(sso_sources[0], AL_MAX_DISTANCE, &max_distance);
	alGetSourcef(sso_sources[0], AL_MIN_GAIN, &min_gain);
	alGetSourcef(sso_sources[0], AL_MAX_GAIN, &max_gain);
	alGetSourcef(sso_sources[0], AL_ROLLOFF_FACTOR, &rolloff_factor);
	alGetSourcef(sso_sources[0], AL_REFERENCE_DISTANCE, &reference_distance);
	alGetSourcef(sso_sources[0], AL_CONE_OUTER_GAIN, &cone_outer_gain);
	alGetSourcef(sso_sources[0], AL_CONE_INNER_ANGLE, &cone_inner_angle);
	alGetSourcef(sso_sources[0], AL_CONE_OUTER_ANGLE, &cone_outer_angle);
	alGetSourcefv(sso_sources[0], AL_POSITION, position);
	alGetSourcefv(sso_sources[0], AL_VELOCITY, velocity);
	alGetSourcefv(sso_sources[0], AL_DIRECTION, direction);

	std::cout << "SoundSourceObject:" << std::endl;
	std::cout << "  Pitch: " << pitch << std::endl;
	std::cout << "  Gain: " << gain << std::endl;
	std::cout << "  Looping: " << (looping ? "true" : "false") << std::endl;
	std::cout << "  Max Distance: " << max_distance << std::endl;
	std::cout << "  Min Gain: " << min_gain << std::endl;
	std::cout << "  Max Gain: " << max_gain << std::endl;
	std::cout << "  Rolloff Factor: " << rolloff_factor << std::endl;
	std::cout << "  Reference Distance: " << reference_distance << std::endl;
	std::cout << "  Cone Outer Gain: " << cone_outer_gain << std::endl;
	std::cout << "  Cone Inner Angle: " << cone_inner_angle << std::endl;
	std::cout << "  Cone Outer Angle: " << cone_outer_angle << std::endl;
	std::cout << "  Position: (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
	std::cout << "  Velocity: (" << velocity[0] << ", " << velocity[1] << ", " << velocity[2] << ")" << std::endl;
	std::cout << "  Direction: (" << direction[0] << ", " << direction[1] << ", " << direction[2] << ")" << std::endl;
}
