#version 330 core

in vec2 i_Uv;
out vec4 o_Color;

uniform sampler2D u_Texture;

void main() {
	vec4 texture_color = texture(u_Texture, i_Uv);

	o_Color = vec4(texture_color.xyz, 1.0);
}
