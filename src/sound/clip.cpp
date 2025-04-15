#include "clip.hpp"

SoundClip::SoundClip(){
	alGetError();
	alGenBuffers(1, &buffer);
	
	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR){
		std::cerr << ("Clip -> SCinit: Failed to generate buffers\f");  //throw exception
		return;
	}
}

SoundClip::~SoundClip(){
	alDeleteBuffers(1, &buffer);
}

void SoundClip::loadOGGFile(const char* filename) {
	// clean up errors
	alGetError();

	int channels, sample_rate;
	short* output;

	// get output, channels, sample rate from filename and size of the data
	ALsizei size_block = stb_vorbis_decode_filename(filename, &channels, &sample_rate, &output);

	if (size_block == -1) {
		std::cerr << ("SoundClip -> OGG file load error\f");
		return;
	}
	this->path = filename;


	if (channels == 2) {
		short* output2 = new short[size_block];

		for (int i = 0;i < size_block;i++) {
			output2[i] = output[i * 2];
		}
		output = output2;
	}

	ALenum format = AL_FORMAT_MONO16;
	ALsizei audio_size = size_block * sizeof(short);
	// Create buffer
	alBufferData(buffer, format, output, audio_size, sample_rate);

	ALenum error;
	if ((error = alGetError()) != AL_NO_ERROR) {
		std::cerr << ("Clip -> addClip: Failed to load file to buffer\f");  //throw exception
		return;
	}

	printf("SoundClip -> OGG file loaded\f");
	free(output);
}

///@TODO WIP
void SoundClip::loadWAVFile(const char* filename) {
	alGetError();

	struct WAVFile {

		char chunk_ID[4];
		uint32_t chunk_size;
		char format[4];

		char subchunk1_ID[4];
		uint32_t subchunk1_size;
		uint16_t audio_format;
		uint16_t num_channels;
		uint32_t sample_rate;
		uint16_t block_align;
		uint16_t bits_per_sample;

		char subchunk2_ID[4];
		uint32_t subchunk2_size;
	};

	std::ifstream wav_file(filename, std::ios::binary);
	WAVFile wav_header;
	wav_file.read(reinterpret_cast<char*>(&wav_header), sizeof(WAVFile));


	///@TODO enable only 1 chanel format
	ALenum format;
	if (wav_header.num_channels == 1) {
		if (wav_header.bits_per_sample == 8) {
			format = AL_FORMAT_MONO8;
		}
		else if (wav_header.bits_per_sample == 16) {
			format = AL_FORMAT_MONO16;
		}
	}
	else if (wav_header.num_channels == 2) {
		if (wav_header.bits_per_sample == 8) {
			format = AL_FORMAT_STEREO8;
		}
		else if (wav_header.bits_per_sample == 16) {
			format = AL_FORMAT_STEREO16;
		}
	}
	//@TODO WiP
}

void SoundClip::loadAudio(const char* filename){
	std::ifstream input_file(filename, std::ios::binary);
	if (!input_file) {
		std::cerr << ("SoundClip -> addAudio: File not found\f");
		return;
	}
	char header[4];
	input_file.read(header, 4);
	input_file.close();

	if (std::string(header, 4) == "OggS") {
		loadOGGFile(filename);
	}	
	else if (std::string(header, 4) == "RIFF" || std::string(header, 4) == "WAVE") 
		std::cerr << ("SoundClip -> addAudio: WAV File not supported\f");
	else
		std::cerr << ("SoundClip -> addAudio: File not supported\f");
}

ALuint SoundClip::getBuffer() {
	return buffer;
}

std::string SoundClip::getPath()
{
	return path;
}