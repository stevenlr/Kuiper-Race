#version 330

in vec2 v_TextureCoords;

out vec4 out_Color;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_EmitTexture;
uniform sampler2D u_SpecularTexture;

void main()
{
	vec3 ambient = vec3(1.0, 0.2, 0.15);
	float ambientFactor = 0.1;

	vec3 diffuse = texture(u_DiffuseTexture, v_TextureCoords).rgb;
	float diffuseFactor = 1 - ambientFactor;

	out_Color = vec4(ambient * ambientFactor + diffuse * diffuseFactor, 1);
}

