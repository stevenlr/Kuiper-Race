#include "Logger.h"

#include "graphics/Registry.h"
#include "graphics/Mesh.h"

#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/VertexAttrib.h"
#include "graphics/opengl/Sampler.h"

#include "Level.h"

void Level::generate_test()
{
	// simulate random timing for the asteroids' rotation
	time = 1000 * ((float) rand()) / RAND_MAX;

	segments.push_back(new Segment());
	segments.push_back(new Segment());
	segments.push_back(new Segment());

	for (Segment *seg : segments) {
		seg->generate_test(Vector3(), Vector3());
	}
}

void Level::update(float dt)
{
	time += dt;
}

void Level::draw(TransformPipeline& tp)
{
	static Sampler samplerMipmap(Sampler::MinLinearMipmapLinear, Sampler::MagLinear, Sampler::Repeat);
	Mesh& sphere = *Registry::models["asteroid"];
	ShaderProgram& shader = *Registry::shaders["test"];

	tp.identity();
	shader.bind();
	shader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	shader["u_DiffuseTexture"].set1i(1);
	shader["u_NormalTexture"].set1i(2);
	shader["u_EmitTexture"].set1i(3);
	shader["u_SpecularTexture"].set1i(4);
	shader["u_Time"].set1f(time);

	Registry::textures["asteroid-diffuse"]->bind(1);
	samplerMipmap.bind(1);
	Registry::textures["asteroid-normal"]->bind(2);
	samplerMipmap.bind(2);
	Registry::textures["asteroid-emit"]->bind(3);
	samplerMipmap.bind(3);
	Registry::textures["asteroid-specular"]->bind(4);
	samplerMipmap.bind(4);

	for (Segment *seg : segments) {
		int n = seg->getAsteroids().size();
		Buffer& buffer = seg->getDataBuffer();

		sphere.addAttrib(4, VertexAttrib(&buffer, 3, VertexAttrib::Float, false, sizeof(Asteroid),
										 reinterpret_cast<const void*>(offsetof(Asteroid, position)), 1));
		sphere.addAttrib(5, VertexAttrib(&buffer, 1, VertexAttrib::Float, false, sizeof(Asteroid),
										 reinterpret_cast<const void*>(offsetof(Asteroid, scale)), 1));
		sphere.addAttrib(6, VertexAttrib(&buffer, 3, VertexAttrib::Float, false, sizeof(Asteroid),
										 reinterpret_cast<const void*>(offsetof(Asteroid, rotation)), 1));

		sphere.drawInstanced(n);
	}
}


