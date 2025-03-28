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
	//parameters
	float sso_pitch = 1.0f;
	float sso_gain = 1.0f;
	bool sso_looping = false;

	//movement
	glm::vec3 sso_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sso_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sso_direction = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<std::weak_ptr<SoundSourceObject>> observers_vector_sso;
	void notifyObserversSSO(void (SoundSourceObject::* method)());
public:
	void addObserversSoundSourceObject(std::shared_ptr<SoundSourceObject> sso);

	SoundGroup& operator= (const SoundGroup& right) {
		if (this != &right) {
			observers_vector_sso = right.observers_vector_sso;
		}
		return *this;
	};


	//===============================Parameters===============================
	//getter
	float getPitch() { return sso_pitch; }
	float getGain() { return sso_gain; }
	bool getLooping() { return sso_looping; }


	//setter
	void setPitch(float pitch);
	void setGain(float gain);
	void setLooping(bool looping);


	//===============================Movements===============================
	//getter
	glm::vec3 getPositionv3() { return sso_position; }
	glm::vec3 getVelocityv3() { return sso_velocity; }
	glm::vec3 getDirectionv3() { return sso_direction; }

	float* getPositionfv() { return &sso_position[0]; }
	float* getVelocityfv() { return &sso_velocity[0]; }
	float* getDirectionfv() { return &sso_direction[0]; }


	//setter
	void setPosition(glm::vec3 position);
	void setVelocity(glm::vec3 velocity);
	void setDirection(glm::vec3 direction);


	~SoundGroup() { observers_vector_sso.clear(); }
};