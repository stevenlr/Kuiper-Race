#include <al.h>
#include <alc.h>
#include <stdlib.h>

#include "Audio.h"
#include "Logger.h"

#define WAVLOADER_HEADER_ONLY
#include "wavloader.c"

#ifndef NDEBUG
#define audio_check_error() do { \
	ALint error; \
	while ((error = alGetError()) != AL_NO_ERROR) { \
		LOGERROR << "Audio " << alGetString(error) << std::endl; \
	} \
} while (false)
#else
#define audio_check_error()
#endif

ALCcontext* Audio::context;
ALCdevice* Audio::device;
std::map<std::string, ALuint> Audio::buffers;
std::vector<ALuint> Audio::sources;

void Audio::initialize()
{
	device = alcOpenDevice(nullptr);
	if (!device) {
		LOGERROR << "Cannot open device" << std::endl;
		return;
	}

	context = alcCreateContext(device, nullptr);
	if (!context) {
		LOGERROR << "Cannot create context" << std::endl;
		return;
	}

	if (!alcMakeContextCurrent(context)) {
		LOGERROR << "Cannot make context" << std::endl;
		return;
	}

	for (unsigned i = 0; i < 16; i++) {
		ALuint source;
		alGenSources(1, &source);
		sources.push_back(source);
	}

	audio_check_error();
}

void Audio::cleanup()
{
	for (auto source : sources) {
		alSourcei(source, AL_BUFFER, 0);
		alDeleteSources(1, &source);
	}
	sources.clear();

	for (auto it : buffers) {
		alDeleteBuffers(1, &it.second);
	}
	buffers.clear();

	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void Audio::load(const std::string& filename, const std::string& name)
{
	struct wave_header wave_header;
	void *buffer = NULL;

	if (load_wav(filename.c_str(), &wave_header, &buffer) < 0) {
		LOGERROR << "Can't load " << filename << std::endl;
		return;
	}
	if (wave_header.bits_per_sample != 8 && wave_header.bits_per_sample != 16) {
		free(buffer);
		LOGERROR << "Can't load " << filename << std::endl;
		return;
	}
	if (wave_header.num_channels != 1 && wave_header.num_channels != 2) {
		free(buffer);
		LOGERROR << "Can't load " << filename << std::endl;
		return;
	}
	if (wave_header.sample_rate != 44100) {
		free(buffer);
		LOGERROR << "Can't load " << filename << std::endl;
		return;
	}

	unsigned int bits_per_sample = wave_header.bits_per_sample;
	unsigned int num_channels = wave_header.num_channels;

	ALenum format = AL_FORMAT_MONO8;
	if (bits_per_sample == 8) {
		if (num_channels == 1) {
			format = AL_FORMAT_MONO8;
		} else if (num_channels == 2) {
			format = AL_FORMAT_STEREO8;
		}
	} else if (bits_per_sample == 16) {
		if (num_channels == 1) {
			format = AL_FORMAT_MONO16;
		} else if (num_channels == 2) {
			format = AL_FORMAT_STEREO16;
		}
	}

	ALuint alBuffer;
	alGenBuffers(1, &alBuffer);
	alBufferData(alBuffer, format, buffer, wave_header.data_size, wave_header.sample_rate);
	audio_check_error();

	free(buffer);
	buffers[name] = alBuffer;
}

ALuint& Audio::getFreeSource()
{
	for (auto& source : sources) {
		ALint status;
		alGetSourcei(source, AL_SOURCE_STATE, &status);
		if (status != AL_PLAYING) {
			return source;
		}
	}

	ALuint source;
	alGenSources(1, &source);
	sources.push_back(source);

	return sources[sources.size() - 1];
}

void Audio::play(const std::string& name, float x, float y, float z)
{
	ALuint& source = getFreeSource();

	alSourcei(source, AL_BUFFER, buffers[name]);
	alSource3f(source, AL_POSITION, x, y, z);
	audio_check_error();

	alSourcePlay(source);
	audio_check_error();
}
