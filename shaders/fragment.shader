#version 310 es
#undef lowp
#undef mediump
#undef highp

precision mediump float;

in vec2 uv_out;
in vec3 diffuse_out;
out vec3 color;

void main()
{
	float color_val = 1.0;

	//draw the checker pattern everywhere
	if (bool(int(int(uv_out.x * 4.0) % 2 == 0) ^ int(int(uv_out.y * 4.0) % 2 == 0)))
		color_val = 0.95;

	color = vec3(color_val * diffuse_out.x, color_val * diffuse_out.y, color_val * diffuse_out.z);
}
