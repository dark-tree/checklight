#pragma once

#include "external.hpp"

class SceneUniform;

class RenderParameters {

	private:

		int gi_samples = 1;
		int gi_max_sample_age = 200;

		bool denoise = true;
		bool shadows = true;

		glm::vec3 ambient_color = glm::vec3(0.0f);
		glm::vec3 sky_color = glm::vec3(0.0f, 0.1f, 0.3f);
		float sky_emission = 0.2f;
		float gi_brightness = 2.0f;

		glm::vec3 portal_tint = glm::vec3(0.0f);
		int portal_jumps = 1;
		bool portal_gi = false;

	public:

		void updateSceneUniform(SceneUniform& uniform) const;

		int getGISamples() const;
		void setGISamples(int samples);

		float getGIBrightness() const;
		void setGIBrightness(float brightness);

		bool getDenoise() const;
		void setDenoise(bool denoise);

		bool getShadows() const;
		void setShadows(bool shadows);

		glm::vec3 getAmbientLight() const;
		void setAmbientLight(glm::vec3 color);
		
		glm::vec3 getPortalTint() const;
		void setPortalTint(glm::vec3 color);

		int getPortalMaxJumps() const;
		void setPortalMaxJumps(int jumps);

		bool getPortalGIEnable() const;
		void setPortalGIEnable(bool gi);

		glm::vec3 getSkyColor() const;
		void setSkyColor(glm::vec3 color);

		float getSkyEmission() const;
		void setSkyEmission(float emission);

		int getMaxSampleAge() const;
		void setMaxSampleAge(int age);
		
};
