#include "InputHandler.h"

#include "maths/TransformPipeline.h"
#include "graphics/Registry.h"
#include "graphics/opengl/ShaderProgram.h"
#include "graphics/opengl/Sampler.h"

#include "Spaceship.h"

void Spaceship::update(float dt)
{
	InputHandler &input = InputHandler::getInstance();

	//if (input.keyIsDown(InputHandler::SpeedUp)) {
	//	augmenter la vitesse
	//}

	// etc.
}

void Spaceship::draw(TransformPipeline& tp)
{
	static Sampler samplerMipmap(Sampler::MinLinearMipmapLinear, Sampler::MagLinear, Sampler::Repeat);
	ShaderProgram &shader = *Registry::shaders["ship"];

	shader.bind();

	tp.saveModel();
	tp.identity();

	samplerMipmap.bind(1);
	samplerMipmap.bind(2);
	samplerMipmap.bind(3);
	samplerMipmap.bind(4);
	Registry::textures["ship-diffuse"]->bind(1);
	Registry::textures["ship-normal"]->bind(2);
	Registry::textures["ship-emit"]->bind(3);
	Registry::textures["ship-specular"]->bind(4);

	shader["u_PvmMatrix"].setMatrix4(tp.getPVMMatrix());
	shader["u_NormalMatrix"].setMatrix3(tp.getNormalMatrix());
	shader["u_ViewMatrix"].setMatrix4(tp.getViewMatrix());
	shader["u_ViewModelMatrix"].setMatrix4(tp.getViewModelMatrix());
	Registry::models["ship"]->draw();

	tp.restoreModel();

	shader.unbind();
}

void Spaceship::applyLookAt(TransformPipeline& tp)
{
	static Camera camera({-3, 0.5, 0.5});
	camera.update();
	tp.lookAt(camera);
}

const Vector3& Spaceship::getPosition() const
{
	return position;
}

const Vector3& Spaceship::getSpeed() const
{
	return speed;
}

float Spaceship::getRadius() const
{
	return 1.f;
}

