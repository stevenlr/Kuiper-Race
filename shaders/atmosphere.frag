#version 330
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TextureCoords;
in vec3 v_LightDir;

out vec4 out_Color;

uniform sampler2D u_Texture;

void main()
{
	vec3 color = vec3(0.8, 0.8, 0.2);
	float factor = 1 - max(0, dot(normalize(v_Position), -v_Normal));

	out_Color = vec4(color * factor, 1);
}

