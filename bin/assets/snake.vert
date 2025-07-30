#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_Uv;

out vec2 i_Uv;

uniform vec2 u_Offset;
uniform vec2 u_Velocity;

void main() {
	i_Uv = a_Uv;

	mat2 nodespace_matrix = mat2(
		 u_Velocity.x, u_Velocity.y,
		-u_Velocity.y, u_Velocity.x
	);

	vec2 nodespace = nodespace_matrix * a_Position;
	vec2 snakespace = nodespace + u_Offset;
	vec2 screenspace = snakespace * 2 - 1;

	gl_Position = vec4(screenspace, 0, 1.0);
}
