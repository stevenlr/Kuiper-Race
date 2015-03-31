#ifndef RECTANGLE_H
#define RECTANGLE_H

class VertexArray;
class Buffer;

class Rectangle
{
	VertexArray* vao;
	Buffer* coordsBuffer;
	Buffer* colorsBuffer;
	Buffer* indicesBuffer;

public:
	Rectangle(float x, float y, float w, float h, float r, float g, float b);
	~Rectangle();

	void draw();
	void setBounds(float x, float y, float w, float h);
	void setColor(float r, float g, float b);
};

#endif

