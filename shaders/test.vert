#version 330

// vertex:
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec2 in_TextureCoords;
// instance:
in vec3 in_InstancePosition;
in float in_InstanceScale;
in vec3 in_InstanceRotation;

out vec3 v_Position;
out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_LightDir;

uniform mat4 u_PvmMatrix;
uniform mat3 u_NormalMatrix;
uniform mat4 u_ViewModelMatrix;
uniform mat4 u_ViewMatrix;
uniform float u_Time;

mat4 computeRotationMatrix(vec3 angles)
{
	vec3 s = sin(angles);
	vec3 c = cos(angles);
	mat4 mat = mat4(1);

	mat *= mat4(
		1, 0, 0, 0,
		0, c.x, s.x, 0,
		0, -s.x, c.x, 0,
		0, 0, 0, 1
	);

	mat *= mat4(
		c.y, 0, -s.y, 0,
		0, 1, 0, 0,
		s.y, 0, c.y, 0,
		0, 0, 0, 1
	);

	mat *= mat4(
		c.y, -s.y, 0, 0,
		s.y, c.y, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	return mat;
}

void main()
{
	mat4 rotationMatrix = computeRotationMatrix(in_InstanceRotation * u_Time);
	vec3 position = (in_InstanceScale * rotationMatrix * vec4(in_Position, 1)).xyz + in_InstancePosition;
	gl_Position = u_PvmMatrix * vec4(position, 1);

	v_Position = (u_ViewModelMatrix * vec4(position, 1)).xyz;
	v_TextureCoords = in_TextureCoords;
	v_Normal = normalize(u_NormalMatrix * in_InstanceScale * mat3(rotationMatrix) * in_Normal);
	v_Tangent = normalize((u_ViewModelMatrix * in_InstanceScale * rotationMatrix * vec4(in_Tangent, 0)).xyz);

	v_LightDir = normalize((u_ViewMatrix * vec4(0, -1, 0, 0)).xyz);
}

