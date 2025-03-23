#include "clip.hpp"

SoundClip::SoundClip(){
	alGetError();
	this->number_of_buffers = 1;
	sc_buffers = new ALuint[number_of_buffers];
	alGenBuffers(1, sc_buffers);
	
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR){
		std::cerr << ("Clip -> SCinit: Failed to generate buffers\f");  //throw exception
		return;
	}
}

//SoundClip::SoundClip(int number_of_buf){
//	alGetError();
//	this->number_of_buffers = number_of_buf;
//	sc_buffers = new ALuint[number_of_buffers];
//	alGenBuffers(number_of_buf, sc_buffers);
//	ALenum error;
//	if ((error = alGetError()) != AL_NO_ERROR){
//		throw std::runtime_error("Clip -> SCinit: Failed to generate buffers\f");  //throw exception
//	}
//}

SoundClip::~SoundClip(){
	alDeleteBuffers(this->number_of_buffers, this->sc_buffers);
	free(sc_buffers);
}

void SoundClip::addAudio(const char* filename){
	// clean up errors
	alGetError();

	int channels,sample_rate;
	short* output;

	// get output, channels, sample rate from filename and size of the data
	ALsizei size_block = stb_vorbis_decode_filename(filename, &channels, &sample_rate, &output);

	if (size_block == -1){
		std::cerr << ("SoundClip -> OGG file load error\f");
		return;
	}
	this->uri = filename;

	ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	ALsizei audio_size = size_block * channels * sizeof(short);

	// Create buffer
	alBufferData(this->sc_buffers[0], format, output, audio_size, sample_rate);

	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR){
		std::cerr << ("Clip -> addClip: Failed to load file to buffer\f");  //throw exception
		return;
	}

	printf("SoundClip -> OGG file loaded\f");
	free(output);
}

//ALuint SoundClip::getBuffer(int number) {
//
//	if (number >= this->number_of_buffers)
//		return 0;
//	return this->sc_buffers[number];
//}

ALuint SoundClip::getBuffer() {
	return this->sc_buffers[0];
}

std::string SoundClip::getURI()
{
	return this->uri;
}