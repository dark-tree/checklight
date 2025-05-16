#include "clip.hpp"

#include "shared/logger.hpp"

SoundClip::SoundClip(){
	alGetError();
	alGenBuffers(1, &buffer);
	
	if (alGetError() != AL_NO_ERROR){
		std::cerr << ("Clip -> SCinit: Failed to generate buffers\n");  //throw exception
		return;
	}
}

SoundClip::~SoundClip(){
	alDeleteBuffers(1, &buffer);
}

void SoundClip::convertChannels(int* audio_size, int channels, short* data) {
	/// Resize to 1 channel
	int new_audio_size = *audio_size / channels;
	/// Get data only from 1 channel
	int j = 0;
	int sum_audio = 0;
	for (int i = 0; i < *audio_size; i++) {
		sum_audio += data[i];
		/// Check if we are at the end of the block of channels
		/// If we are, get average of the channels and save to the output
		/// All channels convert to mono as average of the channels
		if (i % channels == (channels-1)) {
			data[j] = sum_audio/channels;
			sum_audio = 0;
			j++;
		}	
	}
	*audio_size = new_audio_size;
}

void SoundClip::loadOGGFile(const char* filename) {
	// clean up errors
	alGetError();

	int channels, sample_rate;
	short* output;

	// get output, channels, sample rate from filename and size of the data
	ALsizei size_block = stb_vorbis_decode_filename(filename, &channels, &sample_rate, &output);

	if (size_block == -1) {
		out::error("Failed to decode OGG file '%s'!", filename);
		free(output);
		return;
	}
	this->path = filename;

	/// Check of the file has more than 2 channels. If it has, convert it to mono
	if (channels >= 2) {
		convertChannels(&size_block, channels, output);
	}

	ALenum format = AL_FORMAT_MONO16;
	ALsizei audio_size = size_block * sizeof(short);

	// Create buffer
	alBufferData(buffer, format, output, audio_size, sample_rate);
	free(output);

	if (alGetError() != AL_NO_ERROR) {
		FAULT("Failed to upload the sound buffer!");
	}

	out::debug("Loaded sound '%s'", filename);
}

void SoundClip::loadWAVFile(const char* filename) {
	alGetError();
	/// WAV File Header
	/// https://docs.fileformat.com/audio/wav/
	/// http://soundfile.sapp.org/doc/WaveFormat/
	struct WAVFile {
		char chunk_ID[4];
		uint32_t chunk_size;
		char format[4];

		char subchunk1_ID[4];
		uint32_t subchunk1_size;
		uint16_t audio_format;
		uint16_t num_channels;
		uint32_t sample_rate;
		uint32_t byte_rate;
		uint16_t block_align;
		uint16_t bits_per_sample;

		char subchunk2_ID[4];
		uint32_t subchunk2_size;
	};

	std::ifstream wav_file(filename, std::ios::binary);
	WAVFile wav_header;
	/// Read header from file
	if (!wav_file.read(reinterpret_cast<char*>(&wav_header), sizeof(WAVFile))) {
		out::error("Failed decode WAV file '%s', can't read file header!", filename);
		return;
	}

	ALsizei frequency = wav_header.sample_rate;
	/// Read all audio data without header from file
	std::vector<short> data;
	data.resize(wav_header.subchunk2_size);

	if (!wav_file.read(reinterpret_cast<char*>(data.data()), wav_header.subchunk2_size)) {
		out::error("Failed decode WAV file '%s', can't read file chunk!", filename);
		return;
	}
	ALsizei audio_size = wav_header.subchunk2_size;

	/// Check of the file has more than 2 channels. If it has, convert it to mono
	if (wav_header.num_channels >= 2) {
		convertChannels(&audio_size, wav_header.num_channels, data.data());
	}

	ALenum format;
	if (wav_header.bits_per_sample == 8) {
		format = AL_FORMAT_MONO8;
	}
	else if (wav_header.bits_per_sample == 16) {
		format = AL_FORMAT_MONO16;
	} else {
		out::error("Failed decode WAV file '%s', unsupported channel count, expected mono or stereo!", filename);
		return;
	}

	alBufferData(buffer, format, data.data(), audio_size, frequency);

	if (alGetError() != AL_NO_ERROR) {
		FAULT("Failed to upload the sound buffer!");
	}

	out::debug("Loaded sound '%s'", filename);
}

void SoundClip::loadAudio(const char* filename) {
	std::ifstream input_file {filename, std::ios::binary};

	if (!input_file) {
		out::error("Can't open sound file '%s'!", filename);
		return;
	}

	char header[4];
	input_file.read(header, 4);
	input_file.close();

	if (std::string(header, 4) == "OggS") {
		loadOGGFile(filename);
		return;
	}

	if (std::string(header, 4) == "RIFF" || std::string(header, 4) == "WAVE") {
		loadWAVFile(filename);
		return;
	}

	out::error("Can't open sound file '%s', unsupported or invalid format!", filename);

}

ALuint SoundClip::getBuffer() {
	return buffer;
}

std::string SoundClip::getPath()
{
	return path;
}