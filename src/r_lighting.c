#include "r_lighting.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include "r_main.h"

static struct r_RenderObjects lightingRenderObjects;
static unsigned int lightingGridTexture;

void r_InitLighting(const char* shaderName) {
	const float vertices[] = {
		-1, -1, 0, 0,
		-1, +1, 0, 1,
		+1, +1, 1, 1,
		-1, -1, 0, 0,
		+1, -1, 1, 0,
		+1, +1, 1, 1
	};

	if (r_InitObjects(&lightingRenderObjects, 6, shaderName) == 0) {
		printf("Unable to init lighting\n");
		return;
	}
	r_AddVertices(&lightingRenderObjects, vertices, 6);

	glGenTextures(1, &lightingGridTexture);
	glBindTexture(GL_TEXTURE_2D, lightingGridTexture);
	
	const int level = 0;
	const int border = 0;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// NULL for data to indicate no data/base allocation
	glTexImage2D(GL_TEXTURE_2D, level, GL_RED, G_GRID_WIDTH, G_GRID_HEIGHT, border, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void r_FreeLighting() {
	r_FreeObjects(&lightingRenderObjects);
	glDeleteTextures(1, &lightingGridTexture);
}

static void SetGridPixel(unsigned char* gridBitmap, int x, int y, unsigned char value) {
	const int i = x + y * G_GRID_HEIGHT;
	if (i < 0 || i > G_GRID_WIDTH * G_GRID_HEIGHT) {
		return;
	}
	gridBitmap[i] = value;
}

static void UpdateGridTexture() {
	const unsigned char spotEmpty = 0;
	const unsigned char spotSolid = 127;
	const unsigned char spotLight = 255;

	unsigned char gridBitmap[G_GRID_WIDTH * G_GRID_HEIGHT];
	memset(gridBitmap, spotEmpty, sizeof(gridBitmap));

	SetGridPixel(gridBitmap, gameState.appleX, gameState.appleY, spotLight);

	struct g_SnakeNode* ptr = gameState.head;
	while (ptr != NULL) {
		SetGridPixel(gridBitmap, ptr->x, ptr->y, spotSolid);
		ptr = ptr->next;
	}

	glBindTexture(GL_TEXTURE_2D, lightingGridTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, G_GRID_WIDTH, G_GRID_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, gridBitmap);
}

void r_DrawLighting() {
	if (gameState.head != NULL) {
		UpdateGridTexture();
	}

	glBindVertexArray(lightingRenderObjects.vao);
	glUseProgram(lightingRenderObjects.program);
	glDrawArrays(GL_TRIANGLES, 0, lightingRenderObjects.vertices);
}
