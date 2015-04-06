#version 330

in float v_zCoordinate;

out vec4 out_Color;

void main()
{
	out_Color = vec4(1 - v_zCoordinate, 1, v_zCoordinate, 1);
}
