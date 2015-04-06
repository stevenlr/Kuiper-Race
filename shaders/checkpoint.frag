#version 330

uniform vec3 u_Color;

out vec4 out_Color;

void main()
{
	out_Color = vec4(u_Color, 0.7);
}
