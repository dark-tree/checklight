#pragma once

#include "external.hpp"

class SceneUniform;

class RenderParameters {

	private:

		int gi_samples = 1;

		bool denoise = true;
		bool shadows = true;

		glm::vec3 ambient_color = glm::vec3(0.0f);

	public:

		void updateSceneUniform(SceneUniform& uniform) const;

		int getGISamples() const;
		void setGISamples(int samples);

		bool getDenoise() const;
		void setDenoise(bool denoise);

		bool getShadows() const;
		void setShadows(bool shadows);

		void setAmbientLight(glm::vec3 color);
		
};
