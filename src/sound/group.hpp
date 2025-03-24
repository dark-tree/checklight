#pragma once

#include "external.hpp"

class SoundSourceObject;
#include "source.hpp"

//@TODO naprawic include recursive (include guards)
//@TODO dodac observer patern
//@TODO jakos uniezaleznic 2 groupy ale wciaz mialy wspolony kod zeby nie pisac go 2 razy


// shared struct with parameters for SoundSourceObjects
struct SoundGroupParameters {
private:
	float sso_pitch;
	float sso_gain;
	bool sso_looping; 
	
	std::vector<std::weak_ptr<SoundSourceObject>> observers_vector_sso;

public:
	SoundGroupParameters()
		:sso_pitch(1.0f), sso_gain(1.0f), sso_looping(false) {}
	~SoundGroupParameters() { observers_vector_sso.clear(); }

	float getPitch() { return sso_pitch; }
	float getGain() { return sso_gain; }
	bool getLooping() { return sso_looping; }

	void setPitch(float pitch) { sso_pitch = pitch; }
	void setGain(float gain) { sso_gain = gain; }
	void setLooping(bool looping) { sso_looping = looping; }

	void addObserversSoundSourceObject(std::shared_ptr<SoundSourceObject> sso) {
		if (!sso) {
			std::cerr << ("SoundGroupParameters -> addObserversSoundSourceObject: SoundSourceObject not exist\f");
			return;
		}

		// check if exist a source with a given name
		if (find_in_vector(observers_vector_sso, sso) != observers_vector_sso.end()) {
			std::cerr << ("SoundGroupParameters -> addObserversSoundSourceObject: SoundSourceObject already exist\f");
			return;
		}

		observers_vector_sso.push_back(sso); 
	}

};

struct SoundGroupMovement {
private:
	glm::vec3 sso_position;
	glm::vec3 sso_velocity;
	glm::vec3 sso_direction;
public:
	SoundGroupMovement()
		:	sso_position((0.0f, 0.0f, 0.0f)),
			sso_velocity((0.0f, 0.0f, 0.0f)),
			sso_direction((0.0f, 0.0f, 0.0f)) {}

	glm::vec3 getPosition() { return sso_position; }
	glm::vec3 getVelocity() { return sso_velocity; }
	glm::vec3 getDirection() { return sso_direction; }

	void setPosition(glm::vec3 position) { sso_position = position; }
	void setVelocity(glm::vec3 velocity) { sso_velocity = velocity; }
	void setDirection(glm::vec3 direction) { sso_direction = direction; }
};