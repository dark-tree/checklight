#pragma once

#include "external.hpp"

/**
 * Helper for parsing input paramters
 */
class Args {

	private:

		std::string program;
		std::unordered_map<std::string, std::string> args;

	public:

		Args(int argc, const char* argv[]);

		bool has(const char* flag) const;
		const std::string& get(const char* flag) const;
		const std::string& name() const;

};
