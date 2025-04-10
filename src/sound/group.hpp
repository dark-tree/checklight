#pragma once

#include "external.hpp"

class SoundSourceObject;
#include "source.hpp"
#include "functions.hpp"

// shared struct with parameters for SoundSourceObjects


// ===============================SoundGroup===============================


struct SoundGroup {
private:

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

	/// Vector of weak pointers to SoundSourceObjects
	/// Stored every weak pointer to SoundSourceObject that is in this SoundGroup
	/// and would be notify by this SoundGroup when parameters are changed
	std::vector<std::weak_ptr<SoundSourceObject>> observers_vector_sso;

	/// Calls the specified method for each SoundSourceObject in observers_vector_sso
	/// 
	/// @param method Pointer to the method to be called
	void notifyObserversSSO(void (SoundSourceObject::* method)());
public:
	SoundGroup() {};
	~SoundGroup() { observers_vector_sso.clear(); }

	void addObserversSoundSourceObject(std::shared_ptr<SoundSourceObject> sso);

	SoundGroup& operator= (const SoundGroup& right);

	//===============================Parameters===============================
	///getter
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


	///setter
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
	///getter

	/// Get Vec3 position of SoundGroup
	/// 
	/// @return Vec3 position of SoundGroup
	glm::vec3 getPosition() const { return sg_position; }

	/// Get Vec3 velocity of SoundGroup
	/// 
	/// @return Vec3 velocity of SoundGroup
	glm::vec3 getVelocity() const { return sg_velocity; }

	/// Get Vec3 direction of SoundGroup
	/// 
	/// @return Vec3 direction of SoundGroup
	glm::vec3 getDirection() const { return sg_direction; }


	///setter
	void setPosition(glm::vec3 position);
	void setPosition(float x,float y, float z);
	void setVelocity(glm::vec3 velocity);
	void setDirection(glm::vec3 direction);
};