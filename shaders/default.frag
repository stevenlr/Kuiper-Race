#version 330

in vec3 v_Position;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec2 v_TextureCoords;
in vec3 v_LightDir;

out vec4 out_Color;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_EmitTexture;
uniform sampler2D u_SpecularTexture;

vec3 computeNormal()
{
	vec3 normal = normalize(texture(u_NormalTexture, v_TextureCoords).rgb * 2 - 1);
	vec3 bitangent = cross(v_Tangent, v_Normal);

	return mat3(v_Tangent.x, v_Tangent.y, v_Tangent.z,
				bitangent.x, bitangent.y, bitangent.z,
				v_Normal.x, v_Normal.y, v_Normal.z) * normal;
}

void main()
{
	vec3 sunColor = vec3(1, 0.98, 0.88);
	vec3 normal = computeNormal();

	const float ambientFactor = 0.0;

	vec3 diffuseColor = texture(u_DiffuseTexture, v_TextureCoords).rgb;
	float diffuseFactor = max(0, dot(-v_LightDir, normal)) * (1 - ambientFactor);

	float specularIntensity = texture(u_SpecularTexture, v_TextureCoords).r;
	const vec3 specularColor = vec3(1, 1, 1);
	float specularFactor = pow(max(0, dot(reflect(-v_LightDir, normal), normalize(v_Position))), 8) * specularIntensity;

	vec3 color1 = vec3(0.4, 0.6, 0.9);
	vec3 color2 = vec3(0.7, 0.9, 0.2);
	vec3 colorPlanet = (color1 + color2) / 2;

	float diffuseFactor2 = max(0, dot(v_LightDir, normal)) * 0.1;

	vec3 emit = texture(u_EmitTexture, v_TextureCoords).rgb;

	out_Color = vec4(
		diffuseColor * (diffuseFactor + ambientFactor) * sunColor
		+ specularFactor * specularColor
		+ colorPlanet * diffuseFactor2 * diffuseColor
		+ emit
	, 1);
}

