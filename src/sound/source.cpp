#include "source.hpp"

#include "shared/logger.hpp"
#include "debug.hpp"

SoundSourceObject::SoundSourceObject() {
	alGetError();
	alGenSources(1, &source);
	if (alGetError() != AL_NO_ERROR) {
		FAULT("Failed to generate sound source!");
	}

	updateParameters();
	updateMovement();
}

SoundSourceObject::~SoundSourceObject() {
	alDeleteSources(1, &source);
	sc_buffer.reset();
	sso_sg.reset();
}

ALuint SoundSourceObject::getSource() {
	if (alIsSource(source) == AL_TRUE) {
		return source;
	}
	return 0;
}

//========================MOVEMENT========================

void SoundSourceObject::setPosition(float x, float y, float z) {
	sso_position = {x, y, z};
	updateMovement();
}

void SoundSourceObject::setPosition(const glm::vec3& position) {
	sso_position = position;
	updateMovement();
}

void SoundSourceObject::setVelocity(float x, float y, float z) {
	sso_velocity = {x, y, z};
	updateMovement();
}

void SoundSourceObject::setVelocity(const glm::vec3& position) {
	sso_velocity = position;
	updateMovement();
}

void SoundSourceObject::setDirection(const glm::vec3& direction) {
	sso_direction = direction;
	updateMovement();
}

void SoundSourceObject::setDirection(float x, float y, float z) {
	sso_direction = {x, y, z};
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

void SoundSourceObject::updatePosition() {
	if (this->sso_sg != nullptr) {
		alSource3f(source, AL_POSITION, sso_position.x + sso_sg->getPosition().x,
		           sso_position.y + sso_sg->getPosition().y, sso_position.z + sso_sg->getPosition().z);
	} else {
		alSource3f(source, AL_POSITION, sso_position.x, sso_position.y, sso_position.z);
	}
}

void SoundSourceObject::updateVelocity() {
	if (this->sso_sg != nullptr) {
		alSource3f(source, AL_VELOCITY, sso_velocity.x + sso_sg->getVelocity().x,
		           sso_velocity.y + sso_sg->getVelocity().y, sso_velocity.z + sso_sg->getVelocity().z);
	} else {
		alSource3f(source, AL_VELOCITY, sso_velocity.x, sso_velocity.y, sso_velocity.z);
	}
}

void SoundSourceObject::updateDirection() {
	if (this->sso_sg != nullptr) {
		alSource3f(source, AL_DIRECTION, sso_direction.x + sso_sg->getDirection().x,
		           sso_direction.y + sso_sg->getDirection().y, sso_direction.z + sso_sg->getDirection().z);
	} else {
		alSource3f(source, AL_DIRECTION, sso_direction.x, sso_direction.y, sso_direction.z);
	}
}

//========================PARAMETERS========================

void SoundSourceObject::setMute(bool mute) {
	sso_is_mute = mute;
	updateMute();
}

void SoundSourceObject::setPitch(float pitch) {
	sso_pitch = pitch;
	updatePitch();
}

void SoundSourceObject::setGain(float gain) {
	sso_gain = gain;
	updateGain();
}

void SoundSourceObject::setLooping(bool looping) {
	sso_looping = looping;
	updateLooping();
}

void SoundSourceObject::setMaxDistance(float max_distance) {
	sso_max_distance = max_distance;
	updateMaxDistance();
}

void SoundSourceObject::setMinGain(float min_gain) {
	sso_min_gain = min_gain;
	updateGain();
}

void SoundSourceObject::setMaxGain(float max_gain) {
	sso_max_gain = max_gain;
	updateGain();
}

void SoundSourceObject::setRolloffFactor(float rolloff_factor) {
	sso_rolloff_factor = rolloff_factor;
	updateRolloffFactor();
}

void SoundSourceObject::setReferenceDistance(float reference_distance) {
	sso_reference_distance = reference_distance;
	updateReferenceDistance();
}

void SoundSourceObject::setConeOuterGain(float cone_outer_gain) {
	sso_cone_outer_gain = cone_outer_gain;
	updateConeOuterGain();
}

void SoundSourceObject::setConeInnerAngle(float cone_inner_angle) {
	sso_cone_inner_angle = cone_inner_angle;
	updateConeInnerAngle();
}

void SoundSourceObject::setConeOuterAngle(float cone_outer_angle) {
	sso_cone_outer_angle = cone_outer_angle;
	updateConeOuterAngle();
}

//========================UPDATE========================

void SoundSourceObject::updateMute() {
	if (this->sso_sg != nullptr) {
		if (sso_sg->getMute() || sso_is_mute) {
			alSourcef(source, AL_GAIN, 0);
		} else {
			alSourcef(source, AL_GAIN, sso_sg->getGain() * sso_gain);
		}
	} else {
		if (sso_is_mute) {
			alSourcef(source, AL_GAIN, 0);
		} else {
			alSourcef(source, AL_GAIN, sso_gain);
		}
	}
}

void SoundSourceObject::updatePitch() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_PITCH, sso_sg->getPitch() * sso_pitch);
	} else {
		alSourcef(source, AL_PITCH, sso_pitch);
	}
}

void SoundSourceObject::updateGain() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_GAIN, sso_sg->getGain() * sso_gain);
	} else {
		alSourcef(source, AL_GAIN, sso_gain);
	}
}

void SoundSourceObject::updateLooping() {
	if (this->sso_sg != nullptr) {
		alSourcei(source, AL_LOOPING, sso_sg->getLooping() || sso_looping);
	} else {
		alSourcei(source, AL_LOOPING, sso_looping);
	}
}

void SoundSourceObject::updateMaxDistance() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_MAX_DISTANCE, sso_sg->getMaxDistance() + sso_max_distance);
	} else {
		alSourcef(source, AL_MAX_DISTANCE, sso_max_distance);
	}
}

void SoundSourceObject::updateMinMaxGain() {
	if (this->sso_sg != nullptr) {
		float max_gain = (sso_sg->getMaxGain() > sso_max_gain) ? sso_sg->getMaxGain() : sso_max_gain;
		float min_gain = (sso_sg->getMinGain() <= sso_min_gain) ? sso_sg->getMinGain() : sso_min_gain;

		if (min_gain > max_gain) {
			out::warn("Sound min gain is bigger than max gain!");
			min_gain = max_gain;
		}

		alSourcef(source, AL_MAX_GAIN, max_gain);
		alSourcef(source, AL_MIN_GAIN, min_gain);
	} else {
		alSourcef(source, AL_MIN_GAIN, sso_min_gain);
		alSourcef(source, AL_MAX_GAIN, sso_max_gain);
	}
}

void SoundSourceObject::updateRolloffFactor() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_ROLLOFF_FACTOR, sso_sg->getRolloffFactor() * sso_rolloff_factor);
	} else {
		alSourcef(source, AL_ROLLOFF_FACTOR, sso_rolloff_factor);
	}
}

void SoundSourceObject::updateReferenceDistance() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_REFERENCE_DISTANCE, sso_sg->getReferenceDistance() * sso_reference_distance);
	} else {
		alSourcef(source, AL_REFERENCE_DISTANCE, sso_reference_distance);
	}
}

void SoundSourceObject::updateConeOuterGain() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_CONE_OUTER_GAIN, sso_sg->getConeOuterGain() * sso_cone_outer_gain);
	} else {
		alSourcef(source, AL_CONE_OUTER_GAIN, sso_cone_outer_gain);
	}
}

void SoundSourceObject::updateConeInnerAngle() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_CONE_INNER_ANGLE, sso_sg->getConeInnerAngle() + sso_cone_inner_angle);
	} else {
		alSourcef(source, AL_CONE_INNER_ANGLE, sso_cone_inner_angle);
	}
}

void SoundSourceObject::updateConeOuterAngle() {
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_CONE_OUTER_ANGLE, sso_sg->getConeOuterAngle() + sso_cone_outer_angle);
	} else {
		alSourcef(source, AL_CONE_OUTER_ANGLE, sso_cone_outer_angle);
	}
}

//========================BUFFER========================

void SoundSourceObject::addBuffer(SoundClip clip) {
	ALuint buffer = clip.getBuffer();

	if (buffer == 0) {
		FAULT("Invalid sound buffer!");
	}

	alGetError();
	alSourcei(source, AL_BUFFER, buffer);
	alCheckError("during buffer setting!");
}

void SoundSourceObject::addBuffer(std::shared_ptr<SoundClip> clip) {
	if (clip->getBuffer() == 0) {
		FAULT("Invalid clip buffer!");
	}

	alGetError();
	sc_buffer = clip;
	alSourcei(source, AL_BUFFER, sc_buffer->getBuffer());
	alCheckError("during source creation!");
}

//========================SOUND GROUP========================

void SoundSourceObject::addGroupParameters(std::shared_ptr<SoundGroup> sg) {
	this->sso_sg = sg;
	updateParameters();
	updateMovement();
}

void SoundSourceObject::updateParameters() {
	//if sso_sg_parameter has nullptr set default parameters for this SSO
	if (this->sso_sg != nullptr) {
		alSourcef(source, AL_PITCH, sso_sg->getPitch() * sso_pitch);
		alSourcef(source, AL_GAIN, sso_sg->getGain() * sso_gain);
		alSourcei(source, AL_LOOPING, sso_sg->getLooping() || sso_looping);

		float max_gain = (sso_sg->getMaxGain() > sso_max_gain) ? sso_sg->getMaxGain() : sso_max_gain;
		float min_gain = (sso_sg->getMinGain() <= sso_min_gain) ? sso_sg->getMinGain() : sso_min_gain;

		if (min_gain > max_gain) {
			out::warn("Sound min gain is bigger than max gain!");
			min_gain = max_gain;
		}

		alSourcef(source, AL_MAX_GAIN, max_gain);
		alSourcef(source, AL_MIN_GAIN, min_gain);

		alSourcef(source, AL_MAX_DISTANCE, sso_sg->getMaxDistance() + sso_max_distance);
		alSourcef(source, AL_ROLLOFF_FACTOR, sso_sg->getRolloffFactor() * sso_rolloff_factor);
		alSourcef(source, AL_REFERENCE_DISTANCE, sso_sg->getReferenceDistance() * sso_reference_distance);
		alSourcef(source, AL_CONE_OUTER_GAIN, sso_sg->getConeOuterGain() * sso_cone_outer_gain);
		alSourcef(source, AL_CONE_INNER_ANGLE, sso_sg->getConeInnerAngle() + sso_cone_inner_angle);
		alSourcef(source, AL_CONE_OUTER_ANGLE, sso_sg->getConeOuterAngle() + sso_cone_outer_angle);
		if (sso_sg->getMute() || sso_is_mute) {
			alSourcef(source, AL_GAIN, 0);
		}
	} else {
		alSourcef(source, AL_PITCH, sso_pitch);
		alSourcef(source, AL_GAIN, sso_gain);
		alSourcei(source, AL_LOOPING, sso_looping);
		alSourcef(source, AL_MAX_DISTANCE, sso_max_distance);
		alSourcef(source, AL_MIN_GAIN, sso_min_gain);
		alSourcef(source, AL_MAX_GAIN, sso_max_gain);
		alSourcef(source, AL_ROLLOFF_FACTOR, sso_rolloff_factor);
		alSourcef(source, AL_REFERENCE_DISTANCE, sso_reference_distance);
		alSourcef(source, AL_CONE_OUTER_GAIN, sso_cone_outer_gain);
		alSourcef(source, AL_CONE_INNER_ANGLE, sso_cone_inner_angle);
		alSourcef(source, AL_CONE_OUTER_ANGLE, sso_cone_outer_angle);
		if (sso_is_mute) {
			alSourcef(source, AL_GAIN, 0);
		}
	}
}

void SoundSourceObject::updateMovement() {
	if (this->sso_sg != nullptr) {
		glm::vec3 actual_position = sso_position + sso_sg->getPosition();
		alSourcefv(source, AL_POSITION, value_ptr(actual_position));

		glm::vec3 actual_velocity = sso_velocity + sso_sg->getVelocity();
		alSourcefv(source, AL_VELOCITY, value_ptr(actual_velocity));

		glm::vec3 actual_direction = sso_direction + sso_sg->getDirection();
		alSourcefv(source, AL_DIRECTION, value_ptr(actual_direction));
	} else {
		alSourcefv(source, AL_POSITION, value_ptr(sso_position));
		alSourcefv(source, AL_VELOCITY, value_ptr(sso_velocity));
		alSourcefv(source, AL_DIRECTION, value_ptr(sso_direction));
	}
}

//========================CONTROLLS========================

void SoundSourceObject::playSound() {
	if (!alIsSource(source)) {
		FAULT("Invalid source handle!");
	}

	if (!isPlaying()) {
		alGetError();
		alSourcePlay(source);
		alCheckError("while trying to play the source");
	}
}

void SoundSourceObject::stopSound() {
	if (!alIsSource(source)) {
		FAULT("Invalid source handle!");
	}

	alGetError();
	alSourceStop(source);
	alCheckError("while trying to stop the source");
}

void SoundSourceObject::pauseSound() {
	if (!alIsSource(source)) {
		FAULT("Invalid source handle!");
	}

	alGetError();
	alSourcePause(source);
	alCheckError("while trying to pause the source");
}

bool SoundSourceObject::isPlaying() {
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) {
		return false;
	}
	return true;
}

void SoundSourceObject::print() const {
	ALfloat pitch, gain, max_distance, min_gain, max_gain, rolloff_factor, reference_distance, cone_outer_gain,
			cone_inner_angle, cone_outer_angle;
	ALint looping;
	ALfloat position[3], velocity[3], direction[3];

	alGetSourcef(source, AL_PITCH, &pitch);
	alGetSourcef(source, AL_GAIN, &gain);
	alGetSourcei(source, AL_LOOPING, &looping);
	alGetSourcef(source, AL_MAX_DISTANCE, &max_distance);
	alGetSourcef(source, AL_MIN_GAIN, &min_gain);
	alGetSourcef(source, AL_MAX_GAIN, &max_gain);
	alGetSourcef(source, AL_ROLLOFF_FACTOR, &rolloff_factor);
	alGetSourcef(source, AL_REFERENCE_DISTANCE, &reference_distance);
	alGetSourcef(source, AL_CONE_OUTER_GAIN, &cone_outer_gain);
	alGetSourcef(source, AL_CONE_INNER_ANGLE, &cone_inner_angle);
	alGetSourcef(source, AL_CONE_OUTER_ANGLE, &cone_outer_angle);
	alGetSourcefv(source, AL_POSITION, position);
	alGetSourcefv(source, AL_VELOCITY, velocity);
	alGetSourcefv(source, AL_DIRECTION, direction);

	out::info("SoundSourceObject:");
	out::logger.print(" * Pitch: %f\n", pitch);
	out::logger.print(" * Gain: %f\n", gain);
	out::logger.print(" * Looping: %s\n", (looping ? "true" : "false"));
	out::logger.print(" * Max Distance: %f\n", max_distance);
	out::logger.print(" * Min Gain: %f\n", min_gain);
	out::logger.print(" * Max Gain: %f\n", max_gain);
	out::logger.print(" * Rolloff Factor: %f\n", rolloff_factor);
	out::logger.print(" * Reference Distance: %f\n", reference_distance);
	out::logger.print(" * Cone Outer Gain: %f\n", cone_outer_gain);
	out::logger.print(" * Cone Inner Angle: %f\n", cone_inner_angle);
	out::logger.print(" * Cone Outer Angle: %f\n", cone_outer_angle);
	out::logger.print(" * Position: (%f, %f, %f)\n", position[0], position[1], position[2]);
	out::logger.print(" * Velocity: (%f, %f, %f)\n", velocity[0], velocity[1], velocity[2]);
	out::logger.print(" * Direction: (%f, %f, %f)\n", direction[0], direction[1], direction[2]);
}
