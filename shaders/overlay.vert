#version 330

in vec2 in_Position;
in vec3 in_Color;

out vec3 v_Color;

void main()
{
	gl_Position = vec4(in_Position * 2 - 1, 0, 1);
	v_Color = in_Color;
}

