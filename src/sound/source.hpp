#pragma once

#include "external.hpp"
#include "clip.hpp"

class SoundGroup;
#include "group.hpp"

class SoundSourceObject {
private:
	ALuint* sso_sources;
	ALsizei number_of_sources;
	
	std::shared_ptr<SoundGroup> sso_sg;
	
	std::weak_ptr<SoundClip> sc_buffer;

	//parameters
	float sso_pitch = 1.0f;
	float sso_gain = 1.0f;
	bool sso_looping = false;
	bool sso_is_mute = false;
	float sso_max_distance = 100.0f;
	float sso_min_gain = 0.0f;
	float sso_max_gain = 1.0f;
	float sso_rolloff_factor = 1.0f;
	float sso_reference_distance = 1.0f;
	float sso_cone_outer_gain = 0.0f;
	float sso_cone_inner_angle = 360.0f;
	float sso_cone_outer_angle = 360.0f;


	//movement
	glm::vec3 sso_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sso_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sso_direction = glm::vec3(0.0f, 0.0f, 0.0f);

	void initSource(int number_of_sources);
	void play(int source_number);
	void stop(int source_number);
	void pause(int source_number);

public:
	// SoundSourceObject constructor to create object with only one sound source
	SoundSourceObject();

	// SoundSourceObject constructor to create a many sound sources in one object
	//SoundSourceObject(int numberOfSources);

	~SoundSourceObject();


	//========================MOBEMENT========================


	void setPosition(float x, float y, float z);
	void setVelocity(float x, float y, float z);
	void setDirection(float x, float y, float z);

	glm::vec3 getPosition() const { return sso_position; };
	glm::vec3 getVelocity() const { return sso_velocity; };
	glm::vec3 getDirection() const { return sso_direction; };

	//========================PARAMETERS========================

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


	ALuint getSource();
	ALuint getSource(int number);

	void addBuffer(SoundClip buffer);
	void addBuffer(std::shared_ptr<SoundClip> buffer);
	//void addBuffers(SoundClip buffer);

	void addGroupParameters(std::shared_ptr <SoundGroup> sg);
	void updateParameters();
	void updateMovement();

	void playSound();
	void stopSound();
	void pauseSound();

	bool isPlaying();

	void print() const;
};
