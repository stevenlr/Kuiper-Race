#include "graphics/Registry.h"

std::map<std::string, ShaderProgram*> Registry::shaders;
std::map<std::string, Mesh*> Registry::models;
std::map<std::string, Texture*> Registry::textures;

