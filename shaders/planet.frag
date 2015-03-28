#version 330
in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TextureCoords;
in vec3 v_LightDir;

out vec4 out_Color;

uniform sampler2D u_Texture;

void main()
{
	vec3 sunColor = vec3(1, 0.98, 0.88);

	float colorFactor = texture(u_Texture, v_TextureCoords).r;
	vec3 color1 = vec3(0.05, 0.2, 0.4);
	vec3 color2 = vec3(0.5, 0.7, 0.1);
	vec3 color = color1 * colorFactor + color2 * (1 - colorFactor);

	float diffuseFactor = max(0, dot(-v_LightDir, v_Normal));

	float specularIntensity = colorFactor;
	float specularFactor = pow(max(0, dot(reflect(-v_LightDir, v_Normal), normalize(v_Position))), 10) * specularIntensity;

	out_Color = vec4(color * diffuseFactor * sunColor + specularFactor, 1);
}

