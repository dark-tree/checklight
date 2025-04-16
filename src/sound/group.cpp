#include "group.hpp"
#include "sound.hpp"

// ===============================SoundGroup===============================

void SoundGroup::addObserversSoundSourceObject(std::shared_ptr<SoundSourceObject> sso) {
	if (!sso) {
		std::cerr << ("SoundGroupParameters -> addObserversSoundSourceObject: SoundSourceObject not exist\n");
		return;
	}

	// check if exist a source with a given name
	if (findInVector(observers_vector_sso, sso) != observers_vector_sso.end()) {
		std::cerr << ("SoundGroupParameters -> addObserversSoundSourceObject: SoundSourceObject already exist\n");
		return;
	}

	observers_vector_sso.push_back(sso);
}
void SoundGroup::notifyObserversSSO(void (SoundSourceObject::* method)()){
	for (auto it = observers_vector_sso.begin();it != observers_vector_sso.end();) {
		if (auto observer = it->lock()) {
			(observer.get()->*method)();
			++it;
		}
		else {
			it = observers_vector_sso.erase(it);
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

void SoundGroup::setPitch(float pitch) { sg_pitch = pitch; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setGain(float gain) { sg_gain = gain; this->notifyObserversSSO(&SoundSourceObject::updateParameters);}
void SoundGroup::setLooping(bool looping) { sg_looping = looping; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setMute(bool mute) { sg_is_mute = mute; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setMaxDistance(float max_distance) { sg_max_distance = max_distance; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setMinGain(float min_gain) { sg_min_gain = min_gain; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setMaxGain(float max_gain) { sg_max_gain = max_gain; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setRolloffFactor(float rolloff_factor) { sg_rolloff_factor = rolloff_factor; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setReferenceDistance(float reference_distance) { sg_reference_distance = reference_distance; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setConeOuterGain(float cone_outer_gain) { sg_cone_outer_gain = cone_outer_gain; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setConeInnerAngle(float cone_inner_angle) { sg_cone_inner_angle = cone_inner_angle; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }
void SoundGroup::setConeOuterAngle(float cone_outer_angle) { sg_cone_outer_angle = cone_outer_angle; this->notifyObserversSSO(&SoundSourceObject::updateParameters); }

// ===============================SoundGroupMovement===============================

void SoundGroup::setPosition(glm::vec3 position) { sg_position = position; this->notifyObserversSSO(&SoundSourceObject::updateMovement); }
void SoundGroup::setPosition(float x, float z, float y) { sg_position.x = x; sg_position.y = y; sg_position.z = z; this->notifyObserversSSO(&SoundSourceObject::updateMovement); }
void SoundGroup::setVelocity(glm::vec3 velocity) { sg_velocity = velocity; this->notifyObserversSSO(&SoundSourceObject::updateMovement); }
void SoundGroup::setDirection(glm::vec3 direction) { sg_direction = direction; this->notifyObserversSSO(&SoundSourceObject::updateMovement); }