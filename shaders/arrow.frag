#version 330

in vec2 v_TextureCoords;

uniform sampler2D u_DiffuseTexture;

out vec4 out_Color;

void main()
{
	out_Color = vec4(texture(u_DiffuseTexture, v_TextureCoords).rgb * vec3(0.4, 1, 0.4), 1);
}
