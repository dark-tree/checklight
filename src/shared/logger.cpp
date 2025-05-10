//
// Created by magistermaks on 3/22/25.
//

#include "logger.hpp"

/*
 * Logger
 */

const char* Logger::getLevelName(Level level) {
	switch (level) {
		case DEBUG: return "DEBUG";
		case INFO: return "INFO";
		case WARNING: return "WARN";
		case ERROR: return "ERROR";
		default: return "UNKNOWN";
	}

	UNREACHABLE;
}

Logger::Logger(const char* path) {
	file::createPathDirectories(path);
	output = fopen(path, "w");

	if (output == nullptr) {
		error("Failed to open log file '%s' for writing!", path);
	}
}

Logger::~Logger() {
	info("Closing output system...");
	if (output) fclose(output);
}

void Logger::setLogLevelMask(uint32_t mask) {
	this->mask = mask;
}

/*
 * Output
 */
namespace out {

	Logger logger {"debug/output.log"};

}