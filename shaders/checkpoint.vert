#version 330

in vec3 in_Position;

uniform vec3 u_Position;

uniform mat4 u_PvmMatrix;

void main()
{
	float size = 500;
	gl_Position = u_PvmMatrix * vec4(in_Position * size + u_Position, 1);
	gl_PointSize = max(size / (gl_Position.z / 100), 20);
}
