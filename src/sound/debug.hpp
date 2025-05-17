#pragma once
#include <AL/al.h>

const char* alErrorToStr(ALenum error);
void alCheckError(const char* context);