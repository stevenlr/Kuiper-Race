#ifndef REGISTRY_H
#define REGISTRY_H

#include <map>

class ShaderProgram;
class Mesh;

class Registry {
public:
	static std::map<std::string, ShaderProgram*> shaders;
	static std::map<std::string, Mesh*> models;
};

#endif
