#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_Uv;

out vec2 i_Uv;

void main() {
	i_Uv = a_Uv;

	gl_Position = vec4(a_Position.xy, 0, 1.0);
}
