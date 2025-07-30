#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_Uv;

out vec2 i_Uv;

uniform float u_PositionOff;
uniform vec2 u_UvOff;

void main() {
	i_Uv = a_Uv + u_UvOff;;

	gl_Position = vec4(a_Position + vec2(u_PositionOff, 0), 0, 1.0);
}
