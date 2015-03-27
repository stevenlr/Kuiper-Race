#version 330

// vertex:
in vec3 in_Position;
in vec2 in_TextureCoords;
// instance:
in vec3 in_InstancePosition;
in float in_InstanceScale;
in vec3 in_InstanceRotation;

out vec2 v_TextureCoords;

uniform mat4 u_PvmMatrix;
uniform float u_Time;

mat3 computeRotationMatrix(vec3 angles)
{
	vec3 s = sin(angles);
	vec3 c = cos(angles);

	mat3 mat = mat3(1);
	mat *= mat3(
		1, 0, 0,
		0, c.x, s.x,
		0, -s.x, c.x
	);
	mat *= mat3(
		c.y, 0, -s.y,
		0, 1, 0,
		s.y, 0, c.y
	);
	mat *= mat3(
		c.y, -s.y, 0,
		s.y, c.y, 0,
		0, 0, 1
	);
	return mat;
}

void main()
{
	mat3 rotationMatrix = computeRotationMatrix(in_InstanceRotation * u_Time);
	gl_Position = u_PvmMatrix * vec4(in_InstanceScale * rotationMatrix * in_Position + in_InstancePosition, 1);
	v_TextureCoords = in_TextureCoords;
}

