#pragma once

#include "external.hpp"

namespace file {

	/**
	 * Create directories along the file path, expects the last componenet of the path
	 * to be the file even though that component will be ignored.
	 */
	void createPathDirectories(const std::string& file_path);

}