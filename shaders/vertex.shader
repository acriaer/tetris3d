#version 300 es
#undef lowp
#undef mediump
#undef highp

precision mediump float;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 diffuse;
layout(location = 3) in vec3 normal;

uniform bool mode;
uniform mat4 VP;
uniform mat4 M;

out vec2 uv_out;
out vec3 diffuse_out;

void main(){
	vec4 abs_pos = M * vec4(pos, 1.0);

	// if uv == (1,1) and we are in the marker mode we want
	// to make the line go directly down by forcing 0 on the
	// second vertex' y.
	// fixme: too hacky.
	if(mode && uv.x == 1.0 && uv.y == 1.0)
		abs_pos.y = 0.0;

	gl_Position = VP * abs_pos;

	diffuse_out = diffuse;
	uv_out = uv;
}