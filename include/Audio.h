#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <map>
#include <vector>

#include "maths/Vector.h"

#include "al.h"
#include "alc.h"

struct SourceHandler {
	SourceHandler() :
			source(0),
			version(0)
	{}

	SourceHandler(ALuint source, int version) :
			source(source),
			version(version)
	{}

	SourceHandler(const SourceHandler &s) :
		source(s.source),
		version(s.version)
	{}

	ALuint source;
	int version;
};

class Audio {
    static ALCcontext* context;
    static ALCdevice* device;
    static std::map<std::string, ALuint> buffers;
    static std::vector<ALuint> sources;
    static std::vector<int> versions;

    static ALuint& getFreeSource();

public:
    static void initialize();
    static void cleanup();

    static void load(const std::string& filename, const std::string& name);
    static SourceHandler play(const std::string& name, float gain, float pitch, const Vector3 &pos, bool loop = false);
    static SourceHandler play(const std::string& name, float gain = 1.0f, float pitch = 1.0f,
    						  float x = 0.f, float y = 0.f, float z = 0.f,
    						  bool loop = false);
    static void setGain(const SourceHandler &handler, float gain);
    static void setPitch(const SourceHandler &handler, float pitch);
};

#endif

