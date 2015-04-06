#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <map>
#include <vector>

#include "al.h"
#include "alc.h"

class Audio {
    static ALCcontext* context;
    static ALCdevice* device;
    static std::map<std::string, ALuint> buffers;
    static std::vector<ALuint> sources;

    static ALuint& getFreeSource();

public:
    static void initialize();
    static void cleanup();

    static void load(const std::string& filename, const std::string& name);
    static void play(const std::string& name, float x=0.f, float y=0.f, float z=0.f);
};

#endif

