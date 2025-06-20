#pragma once

#include "external.hpp"
#include "clip.hpp"
#include "group.hpp"

struct SoundGroup;

class SoundSourceObject {
	/// Source
	ALuint source; //always 1

	/// Shared pointer to SoundGroup where we get offset for parameters and movement
	std::shared_ptr<SoundGroup> sso_sg;
	/// Weak pointer to SoundClip where we get buffer
	std::shared_ptr<SoundClip> sc_buffer;

	// Parameters for sound source

	float sso_pitch = 1.0f;
	float sso_gain = 1.0f;
	bool sso_looping = false;
	// Mute sound source
	bool sso_is_mute = false;
	float sso_max_distance = 100.0f;
	float sso_min_gain = 0.0f;
	float sso_max_gain = 1.0f;
	float sso_rolloff_factor = 1.0f;
	float sso_reference_distance = 1.0f;
	float sso_cone_outer_gain = 0.0f;
	float sso_cone_inner_angle = 360.0f;
	float sso_cone_outer_angle = 360.0f;

	/// Movement for sound source

	glm::vec3 sso_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sso_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sso_direction = glm::vec3(0.0f, 0.0f, 0.0f);

public:
	/// SoundSourceObject constructor to create object with only one sound source
	SoundSourceObject();

	~SoundSourceObject();

	//========================MOVEMENT========================

	/// Set position of sound source
	void setPosition(float x, float y, float z);

	/// Set position of sound source
	void setPosition(const glm::vec3& position);

	/// Set velocity of sound source
	void setVelocity(float x, float y, float z);

	/// Set velocity of sound source
	void setVelocity(const glm::vec3& position);

	/// Set direction of sound source
	void setDirection(float x, float y, float z);

	/// Set direction of sound source
	void setDirection(const glm::vec3& direction);

	/// Get position of sound source object without offset from sound group
	glm::vec3 getPosition() const { return sso_position; };
	/// Get velocity of sound source object without offset from sound group
	glm::vec3 getVelocity() const { return sso_velocity; };
	/// Get direction of sound source object without offset from sound group
	glm::vec3 getDirection() const { return sso_direction; };

	/// Get position of sound source object with offset from sound group
	glm::vec3 getRealPosition();

	/// Get velocity of sound source object with offset from sound group
	glm::vec3 getRealVelocity();

	/// Get direction of sound source object with offset from sound group
	glm::vec3 getRealDirection();

	/// Update source position with sound group position
	void updatePosition();

	/// Update source velocity with sound group velocity
	void updateVelocity();

	/// Update source direction with sound group direction
	void updateDirection();

	/// Get sound group
	std::shared_ptr<SoundGroup> getSoundGroup() const { return sso_sg; };
	/// Get sound clip
	std::shared_ptr<SoundClip> getSoundClip() const { return sc_buffer; };

	/// Get sound source (first one)
	ALuint getSource();

	//========================PARAMETERS========================

	/// Mute sound source
	void setMute(bool mute);

	void setPitch(float pitch);

	void setGain(float gain);

	void setLooping(bool looping);

	void setMaxDistance(float max_distance);

	void setMinGain(float min_gain);

	void setMaxGain(float max_gain);

	void setRolloffFactor(float rolloff_factor);

	void setReferenceDistance(float reference_distance);

	void setConeOuterGain(float cone_outer_gain);

	void setConeInnerAngle(float cone_inner_angle);

	void setConeOuterAngle(float cone_outer_angle);

	bool getMute() const { return sso_is_mute; };
	float getPitch() const { return sso_pitch; };
	float getGain() const { return sso_gain; };
	bool getLooping() const { return sso_looping; };
	float getMaxDistance() const { return sso_max_distance; };
	float getMinGain() const { return sso_min_gain; };
	float getMaxGain() const { return sso_max_gain; };
	float getRolloffFactor() const { return sso_rolloff_factor; };
	float getReferenceDistance() const { return sso_reference_distance; };
	float getConeOuterGain() const { return sso_cone_outer_gain; };
	float getConeInnerAngle() const { return sso_cone_inner_angle; };
	float getConeOuterAngle() const { return sso_cone_outer_angle; };

	/// Update individual parameter with sound group parameter
	void updateMute();

	void updatePitch();

	void updateGain();

	void updateLooping();

	void updateMaxDistance();

	void updateMinMaxGain();

	void updateRolloffFactor();

	void updateReferenceDistance();

	void updateConeOuterGain();

	void updateConeInnerAngle();

	void updateConeOuterAngle();

	/// Connect sound clip to sound source
	/// 
	/// @param buffer SoundClip object
	void addBuffer(SoundClip buffer);

	/// Connect sound clip to sound source
	///
	/// @param shared pointer to SoundClip object
	void addBuffer(std::shared_ptr<SoundClip> buffer);

	//void addBuffers(SoundClip buffer);

	/// Add sound group to sound source
	/// 
	/// @param shared pointer to SoundGroup object
	void addGroupParameters(std::shared_ptr<SoundGroup> sg);

	/// Update parameters of sound source
	void updateParameters();

	/// Update movement parameters of sound source
	void updateMovement();

	void playSound();

	void stopSound();

	void pauseSound();

	/// Status of playing sound
	bool isPlaying();

	void print() const;
};
