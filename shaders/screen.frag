#version 330

in vec2 v_Position;

uniform sampler2D u_Texture;

out vec4 out_Color;

void main()
{
	vec4 color = texture(u_Texture, v_Position).rgba;

	out_Color = color;
}