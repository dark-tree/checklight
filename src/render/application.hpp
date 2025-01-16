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

		/// Version, as reported to Vulkan Driver
		ApplicationParameters& setVersion(int major, int minor, int patch);

		/// Name, as reported to Vulkan Driver
		ApplicationParameters& setName(const std::string& name);

		/// Initial window dimensions
		ApplicationParameters& setDimensions(int width, int height);

		/// Initial window title
		ApplicationParameters& setTitle(const std::string& title);

};
