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

void main()
{
	gl_Position = u_PvmMatrix * vec4(in_Position + in_InstancePosition, 1);
	v_TextureCoords = in_TextureCoords;
}

