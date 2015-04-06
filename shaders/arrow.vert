#version 330

in vec3 in_Position;

out float v_zCoordinate;

uniform mat4 u_PvmMatrix;

void main()
{
	gl_Position = (u_PvmMatrix * vec4(in_Position, 1)) * vec4(-1, 1, 1, 1);
	v_zCoordinate = in_Position.y;
}
