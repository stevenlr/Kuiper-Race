#ifndef REGISTRY_H
#define REGISTRY_H

#include <string>
#include <map>

#include "graphics/opengl/ShaderProgram.h"
#include "graphics/Mesh.h"
#include "graphics/opengl/Texture.h"
#include "graphics/opengl/Cubemap.h"
#include "graphics/opengl/VertexArray.h"

class Registry {
public:
	static std::map<std::string, ShaderProgram*> shaders;
	static std::map<std::string, Mesh*> models;
	static std::map<std::string, Texture*> textures;
	static Cubemap *cubemap;
	static VertexArray *screenQuad;
	static VertexArray *point;
};

#endif
