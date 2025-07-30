#version 330 core

in vec2 i_Uv;
out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main() {
	vec4 texture_color = texture(u_Texture, i_Uv);

	if (texture_color.xyz == vec3(1, 0, 1)) {
		discard;
	}

	o_Color = vec4(texture_color.xyz * u_Color, 1.0);
}
