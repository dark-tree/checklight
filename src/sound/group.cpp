#include "group.hpp"
#include "sound.hpp"
#include "shared/logger.hpp"

// ===============================SoundGroup===============================

void SoundGroup::addObserversSoundSourceObject(std::shared_ptr<SoundSourceObject> sso) {
	if (!sso) {
		out::warn("There was an attempt to add an invalid source to a group!");
		return;
	}

	// check if a source with a given name exists
	if (findInVector(observers_vector_sso, sso) != observers_vector_sso.end()) {
		out::warn("There was an attempt to add an already preset source to a group!");
		return;
	}

	observers_vector_sso.push_back(sso);
}
void SoundGroup::notifyObserversSSO(void (SoundSourceObject::* method)()){
	removeExpired(observers_vector_sso);

	for (auto it = observers_vector_sso.begin();it != observers_vector_sso.end();) {
		if (auto observer = it->lock()) {
			(observer.get()->*method)();
			++it;
		}
	}
}

SoundGroup& SoundGroup::operator= (const SoundGroup& right) {
	if (this != &right) {
		observers_vector_sso = right.observers_vector_sso;
		this->sg_pitch = right.sg_pitch;
		this->sg_gain = right.sg_gain;
		this->sg_looping = right.sg_looping;
		this->sg_is_mute = right.sg_is_mute;
		this->sg_max_distance = right.sg_max_distance;
		this->sg_min_gain = right.sg_min_gain;
		this->sg_max_gain = right.sg_max_gain;
		this->sg_rolloff_factor = right.sg_rolloff_factor;
		this->sg_reference_distance = right.sg_reference_distance;
		this->sg_cone_outer_gain = right.sg_cone_outer_gain;
		this->sg_cone_inner_angle = right.sg_cone_inner_angle;
		this->sg_cone_outer_angle = right.sg_cone_outer_angle;

		this->sg_position = right.sg_position;
		this->sg_velocity = right.sg_velocity;
		this->sg_direction = right.sg_direction;

		this->notifyObserversSSO(&SoundSourceObject::updateMovement);
		this->notifyObserversSSO(&SoundSourceObject::updateParameters);
	}
	return *this;
};

// ===============================SoundGroupParameters===============================

void SoundGroup::setPitch(float pitch) { sg_pitch = pitch; this->notifyObserversSSO(&SoundSourceObject::updatePitch); }
void SoundGroup::setGain(float gain) { sg_gain = gain; this->notifyObserversSSO(&SoundSourceObject::updateGain);}
void SoundGroup::setLooping(bool looping) { sg_looping = looping; this->notifyObserversSSO(&SoundSourceObject::updateLooping); }
void SoundGroup::setMute(bool mute) { sg_is_mute = mute; this->notifyObserversSSO(&SoundSourceObject::updateMute); }
void SoundGroup::setMaxDistance(float max_distance) { sg_max_distance = max_distance; this->notifyObserversSSO(&SoundSourceObject::updateMaxDistance); }
void SoundGroup::setMinGain(float min_gain) { sg_min_gain = min_gain; this->notifyObserversSSO(&SoundSourceObject::updateMinMaxGain); }
void SoundGroup::setMaxGain(float max_gain) { sg_max_gain = max_gain; this->notifyObserversSSO(&SoundSourceObject::updateMinMaxGain); }
void SoundGroup::setRolloffFactor(float rolloff_factor) { sg_rolloff_factor = rolloff_factor; this->notifyObserversSSO(&SoundSourceObject::updateRolloffFactor); }
void SoundGroup::setReferenceDistance(float reference_distance) { sg_reference_distance = reference_distance; this->notifyObserversSSO(&SoundSourceObject::updateReferenceDistance); }
void SoundGroup::setConeOuterGain(float cone_outer_gain) { sg_cone_outer_gain = cone_outer_gain; this->notifyObserversSSO(&SoundSourceObject::updateConeOuterGain); }
void SoundGroup::setConeInnerAngle(float cone_inner_angle) { sg_cone_inner_angle = cone_inner_angle; this->notifyObserversSSO(&SoundSourceObject::updateConeInnerAngle); }
void SoundGroup::setConeOuterAngle(float cone_outer_angle) { sg_cone_outer_angle = cone_outer_angle; this->notifyObserversSSO(&SoundSourceObject::updateConeOuterAngle); }

// ===============================SoundGroupMovement===============================

void SoundGroup::setPosition(glm::vec3 position) { sg_position = position; this->notifyObserversSSO(&SoundSourceObject::updatePosition); }
void SoundGroup::setPosition(float x, float z, float y) { sg_position.x = x; sg_position.y = y; sg_position.z = z; this->notifyObserversSSO(&SoundSourceObject::updatePosition); }
void SoundGroup::setVelocity(glm::vec3 velocity) { sg_velocity = velocity; this->notifyObserversSSO(&SoundSourceObject::updateVelocity); }
void SoundGroup::setDirection(glm::vec3 direction) { sg_direction = direction; this->notifyObserversSSO(&SoundSourceObject::updateDirection); }