#ifndef REGISTRY_H
#define REGISTRY_H

#include <map>

class ShaderProgram;
class Mesh;
class Texture;

class Registry {
public:
	static std::map<std::string, ShaderProgram*> shaders;
	static std::map<std::string, Mesh*> models;
	static std::map<std::string, Texture*> textures;
};

#endif
