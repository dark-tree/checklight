#pragma once

#include "external.hpp"

class ApplicationParameters {

	private:

		friend class Renderer;

		int major, minor, patch;
		std::string name, title;
		int width, height;

		std::string getName() const;

		std::string getTitle() const;

	public:

		ApplicationParameters();

		static ApplicationParameters begin();

		ApplicationParameters& setVersion(int major, int minor, int patch);

		ApplicationParameters& setName(const std::string& name);

		ApplicationParameters& setDimensions(int width, int height);

		ApplicationParameters& setTitle(const std::string& title);

};
