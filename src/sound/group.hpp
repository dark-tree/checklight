#pragma once

#include "external.hpp"

class SoundSourceObject;
#include "source.hpp"
#include "functions.hpp"

// shared struct with parameters for SoundSourceObjects

//@TODO poprawic kopiowanie obiektow
//@TODO dodac odczepianie od sg w sso

//@TODO SG JAKO srodek ciezkosci a nie parametry stricte dla sso
//@TODO sg to offset parametrow sso
//@TODO sso ma wlasne parametry

// ===============================SoundGroup===============================


struct SoundGroup {
private:

	//@TODO poprawic jakos parametry, bo spoko ze offset ale jak chcemy wylaczyc dzwiek to w sg mamy 0 gain ale tutaj mamy 1.0 i tyle bedzie wciaz gralo
	//		trzeba przemyslec jak to naprawic
	//parameters
	float sg_pitch = 0.0f;
	float sg_gain = 0.0f;
	bool sg_looping = false;
	bool sg_is_mute = false;

	float sg_max_distance = 100.0f;
	float sg_min_gain = 0.0f;
	float sg_max_gain = 1.0f;
	float sg_rolloff_factor = 1.0f;
	float sg_reference_distance = 1.0f;
	float sg_cone_outer_gain = 0.0f;
	float sg_cone_inner_angle = 360.0f;
	float sg_cone_outer_angle = 360.0f;


	//movement
	glm::vec3 sg_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sg_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sg_direction = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<std::weak_ptr<SoundSourceObject>> observers_vector_sso;
	void notifyObserversSSO(void (SoundSourceObject::* method)());
public:
	void addObserversSoundSourceObject(std::shared_ptr<SoundSourceObject> sso);

	SoundGroup& operator= (const SoundGroup& right);

	//===============================Parameters===============================
	//getter
	float getPitch() const { return sg_pitch; }
	float getGain() const { return sg_gain; }
	bool getLooping() const { return sg_looping; }
	bool getMute() const { return sg_is_mute; }

	float getMaxDistance() const { return sg_max_distance; }
	float getMinGain() const { return sg_min_gain; }
	float getMaxGain() const { return sg_max_gain; }
	float getRolloffFactor() const { return sg_rolloff_factor; }
	float getReferenceDistance() const { return sg_reference_distance; }
	float getConeOuterGain() const { return sg_cone_outer_gain; }
	float getConeInnerAngle() const { return sg_cone_inner_angle; }
	float getConeOuterAngle() const { return sg_cone_outer_angle; }


	//setter
	void setPitch(float pitch);
	void setGain(float gain);
	void setLooping(bool looping);
	void setMute(bool mute);

	void setMaxDistance(float max_distance);
	void setMinGain(float min_gain);
	void setMaxGain(float max_gain);
	void setRolloffFactor(float rolloff_factor);
	void setReferenceDistance(float reference_distance);
	void setConeOuterGain(float cone_outer_gain);
	void setConeInnerAngle(float cone_inner_angle);
	void setConeOuterAngle(float cone_outer_angle);



	//===============================Movements===============================
	//getter
	glm::vec3 getPositionv3() const { return sg_position; }
	glm::vec3 getVelocityv3() const { return sg_velocity; }
	glm::vec3 getDirectionv3() const { return sg_direction; }

	const float* getPositionfv() const { return &sg_position[0]; }
	const float* getVelocityfv() const { return &sg_velocity[0]; }
	const float* getDirectionfv() const { return &sg_direction[0]; }


	//setter
	void setPosition(glm::vec3 position);
	void setPosition(float x,float y, float z);
	void setVelocity(glm::vec3 velocity);
	void setDirection(glm::vec3 direction);

	SoundGroup(){};
	~SoundGroup() { observers_vector_sso.clear(); }
};