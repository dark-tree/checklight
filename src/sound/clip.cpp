#include "clip.hpp"

SoundClip::SoundClip()
{
	alGetError();
	this->number_of_buffers = 1;
	sc_buffers = new ALuint[number_of_buffers];
	alGenBuffers(1, sc_buffers);
	
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Clip -> SCinit: Failed to generate buffers");  //throw exception
	}
}

SoundClip::SoundClip(int number_of_buf)
{
	alGetError();
	this->number_of_buffers = number_of_buf;
	sc_buffers = new ALuint[number_of_buffers];
	alGenBuffers(number_of_buf, sc_buffers);
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Clip -> SCinit: Failed to generate buffers");  //throw exception
	}
}

SoundClip::~SoundClip()
{
	alDeleteBuffers(this->number_of_buffers, this->sc_buffers);
}

void SoundClip::addClip(const char* filename)
{
	alGetError();
	int channels,sample_rate;
	short* output;

	ALsizei size_block = stb_vorbis_decode_filename(filename, &channels, &sample_rate, &output);

	if (size_block == -1)
	{
		printf("SoundClip -> OGG file load error\n\f");
		return;
	}
	
	ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	ALsizei audio_size = size_block * channels * sizeof(short);

	alBufferData(this->sc_buffers[0], format, output, audio_size, sample_rate);

	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		throw std::runtime_error("Clip -> addClip: Failed to load file to buffer");  //throw exception
	}

	printf("SoundClip -> OGG file loaded\n\f");
	free(output);
}