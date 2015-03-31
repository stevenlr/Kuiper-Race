#version 330

in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec2 in_TextureCoords;

out vec3 v_Position;
out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_LightDir;

uniform mat4 u_PvmMatrix;
uniform mat3 u_NormalMatrix;
uniform mat4 u_ViewModelMatrix;
uniform mat4 u_ViewMatrix;

void main()
{
	gl_Position = u_PvmMatrix * vec4(in_Position, 1);

	v_Position = (u_ViewModelMatrix * vec4(in_Position, 1)).xyz;
	v_TextureCoords = in_TextureCoords;
	v_Normal = normalize(u_NormalMatrix * in_Normal);
	v_Tangent = normalize((u_ViewModelMatrix * vec4(in_Tangent, 0)).xyz);

	v_LightDir = normalize((u_ViewMatrix * vec4(0, -1, 0, 0)).xyz);
}

