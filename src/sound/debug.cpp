#include "debug.hpp"
#include "shared/logger.hpp"

const char* alErrorToStr(ALenum error) {
	switch (error) {
			case AL_NO_ERROR: return "AL_NO_ERROR";
			case AL_INVALID_NAME: return "AL_INVALID_NAME";
			case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
			case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
			case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
			case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";

			default: return "UNKNOWN";
	}
}


void alCheckError(const char* context) {
	const ALenum error = alGetError();

	if (error != AL_NO_ERROR) {
		const char* value = alErrorToStr(error);

		if (context) {
			out::error("OpenAL Error: %s occurred %s!", value, context);
		} else {
			out::error("OpenAL Error: %s!", value);
		}
	}
}