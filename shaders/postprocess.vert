#version 330

in vec2 in_Position;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

out vec2 v_Position;
out vec3 v_LightDir;
out vec2 v_SunPosition;

void main()
{
	gl_Position = vec4(in_Position * 2 - 1, 0, 1);
	v_Position = in_Position;
	vec4 sunPosition = u_ProjectionMatrix * u_ViewMatrix * vec4(0, 1, 0, 0);
	v_LightDir = normalize(u_ViewMatrix * vec4(0, 1, 0, 0)).xyz;
	v_SunPosition = sunPosition.xy / sunPosition.w;
}