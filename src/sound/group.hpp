#pragma once
// shared struct with parameters for SoundSourceObjects
struct SoundGroup {
private:
	float sso_pitch;
	float sso_gain;
	bool sso_looping;
public:
	SoundGroup()
		:sso_pitch(1.0f), sso_gain(1.0f), sso_looping(false) {}
	float getPitch() { return sso_pitch; }
	float getGain() { return sso_gain; }
	bool getLooping() { return sso_looping; }

	void setPitch(float pitch) { sso_pitch = pitch; }
	void setGain(float gain) { sso_gain = gain; }
	void setLooping(bool looping) { sso_looping = looping; }
};