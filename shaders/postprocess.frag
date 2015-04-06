#version 330

in vec2 v_Position;
in vec3 v_LightDir;
in vec2 v_SunPosition;

uniform sampler2D u_Texture;
uniform sampler2D u_Depth;
uniform float u_Width;
uniform float u_Height;

out vec4 out_Color;

void main()
{
	float depth = texture(u_Depth, v_Position).r;
	vec3 color = vec3(0);
	vec2 posCenter = v_Position * 2 - 1;
	float dist = length(posCenter);

	vec2 posCenterNormalized = posCenter;
	posCenterNormalized.y *= u_Height / u_Width;

	// Abbération chromatique

	vec2 posR = (posCenter * (u_Width + 8) / u_Width + 1) / 2;
	vec2 posG = (posCenter * (u_Width + 4) / u_Width + 1) / 2;
	vec2 posB = v_Position;

	float r = texture2D(u_Texture, posR).r;
	float g = texture2D(u_Texture, posG).g;
	float b = texture2D(u_Texture, posB).b;

	color = vec3(r, g, b);

	// Sun, again

	vec3 viewDir = normalize(vec3(posCenterNormalized, -depth));
	float sunFactor = pow(max(dot(v_LightDir, viewDir), 0), 4);
	color += sunFactor * vec3(0.56, 0.42, 0.26) * 0.53;

	// Light rays

	if (sunFactor > 0) {
		int iterations = 40;
		vec2 start = posCenter;
		vec2 end = v_SunPosition;
		vec2 pas = (end - start) / (iterations * 1.2);
		vec3 c = vec3(0);

		for (int i = 0; i < iterations; ++i) {
			vec2 pos = start + pas * i;

			c += texture(u_Texture, (pos + 1) / 2).rgb / iterations;
		}

		color += c * 0.4 * sunFactor;
	}

	// Vignette

	float vignette = smoothstep(0, 1, mix(0.65, 1.45, 1 - dist));
	color *= vignette;

	out_Color = vec4(color, 1);
}