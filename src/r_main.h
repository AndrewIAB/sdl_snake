#pragma once

#define R_VERTEX_POS   (0)
#define R_VERTEX_UV    (2 * sizeof(float))
#define R_VERTEX_SIZE  (4 * sizeof(float))

#define R_VAO_POS   0
#define R_VAO_UV    1

struct r_RenderObjects {
	unsigned int vbo, vao, program;
	unsigned int vertices;
};

// Return 0 on fail and 1 on success
int r_InitObjects(struct r_RenderObjects* renderObjects, unsigned int preallocVertices, const char* shaderName);
void r_FreeObjects(struct r_RenderObjects* renderObjects);
void r_AddVertices(struct r_RenderObjects* renderObjects, const float* data, unsigned int numVertices);

