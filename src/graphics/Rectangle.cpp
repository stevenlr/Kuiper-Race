#include "graphics/opengl/VertexArray.h"
#include "graphics/opengl/Buffer.h"

#include "graphics/Rectangle.h"


Rectangle::Rectangle(float x, float y, float w, float h, float r, float g, float b) :
	vao(new VertexArray(VertexArray::Triangles, 6)),
	coordsBuffer(new Buffer(Buffer::Array, Buffer::StaticDraw)),
	colorsBuffer(new Buffer(Buffer::Array, Buffer::StaticDraw)),
	indicesBuffer(new Buffer(Buffer::ElementArray, Buffer::StaticDraw))
{
	vao->addAttrib(0, VertexAttrib(coordsBuffer, 2, VertexAttrib::Float));
	vao->addAttrib(1, VertexAttrib(colorsBuffer, 3, VertexAttrib::Float));

	unsigned int indices[] = {0, 1, 3, 3, 1, 2};
	indicesBuffer->data(sizeof(indices), reinterpret_cast<const void*>(indices));
	vao->setElementIndexArray(ElementIndexArray(indicesBuffer));

	setBounds(x, y, w, h);
	setColor(r, g, b);
}

Rectangle::~Rectangle()
{
	delete coordsBuffer;
	delete colorsBuffer;
	delete indicesBuffer;
	delete vao;
}

void Rectangle::draw()
{
	vao->bind();
	vao->drawElements();
	vao->unbind();
}

void Rectangle::setBounds(float x, float y, float w, float h)
{
	float coords[] = {
		x, y,
		x + w, y,
		x + w, y + h,
		x, y + h
	};

	coordsBuffer->data(sizeof(coords), reinterpret_cast<const void*>(coords));
}

void Rectangle::setColor(float r, float g, float b)
{
	float colors[] = {
		r, g, b, // wow
		r, g, b, // much color
		r, g, b, // very rainbow
		r, g, b, // so unexpected
	};

	colorsBuffer->data(sizeof(colors), reinterpret_cast<const void*>(colors));
}

