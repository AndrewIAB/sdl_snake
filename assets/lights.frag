#version 330 core

in vec2 i_Uv;
out vec4 o_Color;

uniform sampler2D u_Texture;

const int PIXEL_EMPTY = 0;
const int PIXEL_SOLID = 1;
const int PIXEL_LIGHT = 2;

const vec3 LIGHT_COLOR = vec3(1, 1, 1);
const float LIGHT_INTENSITY = 8;
const int STEP_NUM = 24;
const float STEP_SIZE = 0.7071 * 0.5;

const int SAMPLE_NUM = 1;

int getGridPixel(vec2 pos) {
	float sample = texture(u_Texture, pos).x;
	return int(round(sample * 2.0));
}

vec2 invVec2(vec2 v) {
	return vec2(1.0 / v.x, 1.0 / v.y);
}

bool outOfBounds(vec2 pos) {
	return pos.x < 0 || pos.x > 1 || pos.y < 0 || pos.y > 1;
}

bool isEmpty(vec2 pos) {
	return !outOfBounds(pos) && getGridPixel(pos) == PIXEL_EMPTY;
}

vec2 getNormal(vec2 pos) {
	float d = 1.0 / textureSize(u_Texture, 0).x / sqrt(2) * 0.5;
	if (isEmpty(pos + vec2(+d, +0))) { return vec2(+1, +0); }
	if (isEmpty(pos + vec2(-d, +0))) { return vec2(-1, +0); }
	if (isEmpty(pos + vec2(+0, +d))) { return vec2(+0, +1); }
	if (isEmpty(pos + vec2(+0, -d))) { return vec2(+0, -1); }
	return vec2(0, 0);
}

vec3 samplePixel(vec2 pos, vec2 dir) {
	vec2 stepUnit = dir * invVec2(textureSize(u_Texture, 0)) * STEP_SIZE;
	float stepDist = length(stepUnit);

	vec3 modLightColor = LIGHT_COLOR * LIGHT_INTENSITY * stepDist;

	vec2 rayPos = pos;
	float distMoved = 0;
	vec3 color = vec3(0, 0, 0);

	for (int i = 0; i < STEP_NUM; i++) {
		int pixel = getGridPixel(rayPos);

		if (outOfBounds(rayPos) || pixel == PIXEL_SOLID) {
			stepUnit *= -getNormal(rayPos);
		} else
		if (pixel == PIXEL_LIGHT) {
			color += modLightColor / sqrt(distMoved);
		}

		rayPos += stepUnit;
		distMoved += stepDist;
	}

	return color;
}

float rand(vec2 co) {
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	vec3 color = vec3(0, 0, 0);

	float coeff = 1.0 / float(SAMPLE_NUM);
	float randVal = rand(i_Uv) * 6.28;
	float angle = 0;

	for (int i = 0; i < SAMPLE_NUM; i++) {
		angle += randVal;
		color += samplePixel(i_Uv, vec2(cos(angle), sin(angle))) * coeff;
	}

	o_Color = vec4(color, 1.0);
}
