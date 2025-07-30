#include "r_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

static char* getShaderPath(const char* shaderName, GLenum type) {
	const char* dir = "assets/";
	const char* ext_frag = ".frag";
	const char* ext_vert = ".vert";

	const char* ext_used = type == GL_VERTEX_SHADER ? ext_vert : ext_frag;

	const size_t ext_size = 5;
	const size_t name_len = strlen(shaderName);
	const size_t buffer_size = sizeof(dir)-1 + ext_size + name_len + 1;

	char* buffer = malloc(buffer_size);
	memcpy(buffer, dir, sizeof(dir)-1);
	memcpy(buffer + sizeof(dir)-1, shaderName, name_len);
	memcpy(buffer + sizeof(dir)-1 + name_len, ext_used, ext_size);
	buffer[buffer_size-1] = '\0';

	return buffer;
}

static int getShader(unsigned int* shader, const char* shaderName, GLenum type) {
	char* shaderPath = getShaderPath(shaderName, type);
	if (shaderPath == NULL) {
		return 0;
	}

	FILE* file = fopen(shaderPath, "r");
	if (file == NULL) {
		free(shaderPath);
		return 0;
	}
	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* shaderSource = malloc(len);
	int readSize = (int)fread(shaderSource, 1, len, file);

	fclose(file);

	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, (const char* const*)&shaderSource, &readSize);
	glCompileShader(*shader);

	char buffer[512];
	int success = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(*shader, 512, NULL, buffer);
		glDeleteShader(*shader);
		printf("%s:\n================================\n%s\n", shaderPath, buffer);
		
		free(shaderPath);
		return 0;
	}
	free(shaderPath);
	return 1;
}

static int getProgram(unsigned int* program, const char* shaderName) {
	unsigned int vert = 0, frag = 0;
	int vert_success = getShader(&vert, shaderName, GL_VERTEX_SHADER);
	int frag_success = getShader(&frag, shaderName, GL_FRAGMENT_SHADER);

	if (!vert_success || !frag_success) {
		glDeleteShader(vert);
		glDeleteShader(frag);
		return 0;
	}

	*program = glCreateProgram();
	glAttachShader(*program, vert);
	glAttachShader(*program, frag);
	glLinkProgram(*program);

	glDeleteShader(vert);
	glDeleteShader(frag);

	return 1;
}

int r_InitObjects(struct r_RenderObjects* renderObjects, unsigned int preallocVertices, const char* shaderName) {
	if (!getProgram(&renderObjects->program, shaderName)) {
		return 0;
	}

	glGenBuffers(1, &renderObjects->vbo);
	glGenVertexArrays(1, &renderObjects->vao);
	renderObjects->vertices = 0;

	glBindVertexArray(renderObjects->vao);
	glBindBuffer(GL_ARRAY_BUFFER, renderObjects->vbo);

	glBufferData(GL_ARRAY_BUFFER, preallocVertices * R_VERTEX_SIZE, NULL, GL_DYNAMIC_DRAW);

	// VAO
	// Position
	glVertexAttribPointer(R_VAO_POS, 2, GL_FLOAT, GL_FALSE, R_VERTEX_SIZE, (void*)(R_VERTEX_POS));
	glEnableVertexAttribArray(R_VAO_POS);
	// Uv
	glVertexAttribPointer(R_VAO_UV, 2, GL_FLOAT, GL_FALSE, R_VERTEX_SIZE, (void*)(R_VERTEX_UV));
	glEnableVertexAttribArray(R_VAO_UV);

	return 1;
}

void r_FreeObjects(struct r_RenderObjects* renderObjects) {
	glDeleteBuffers(1, &renderObjects->vbo);
	glDeleteVertexArrays(1, &renderObjects->vao);
	glDeleteProgram(renderObjects->program);
}

void r_AddVertices(struct r_RenderObjects* renderObjects, const float* data, unsigned int numVertices) {
	const size_t offset = renderObjects->vertices * R_VERTEX_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, renderObjects->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, R_VERTEX_SIZE * numVertices, data);
	renderObjects->vertices += numVertices;
}

