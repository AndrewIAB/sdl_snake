#include "r_game.h"

#include <stddef.h>
#include <math.h>
#include <glad/glad.h>
#include "r_main.h"

static struct r_RenderObjects gameRenderObjects;
static unsigned int gameSnakeBodyTexture;
static unsigned int gameSnakeHeadTexture;
static unsigned int gameAppleTexture;
static int loc_u_Offset;
static int loc_u_Velocity;
static int loc_u_Color;

#define COLOR_CLR 255, 0, 255
#define COLOR_RED 200, 0, 0
#define COLOR_CLEAR COLOR_CLR
#define COLOR_GREEN 0, 200, 0
#define COLOR_BLACK 0, 0, 0

static const char snakeBodyBitmap[] = {
	COLOR_GREEN
};
static const int snakeBodyBitmapSide = 1;

static const char snakeHeadBitmap[] = {
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_CLEAR, COLOR_CLEAR,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_CLEAR,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_BLACK, COLOR_GREEN,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_BLACK, COLOR_GREEN,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_CLEAR,
	COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_CLEAR, COLOR_CLEAR,
};
static const int snakeHeadBitmapSide = 8;

static const char appleBitmap[] = {
	COLOR_CLR, COLOR_RED, COLOR_RED, COLOR_CLR, COLOR_CLR, COLOR_RED, COLOR_RED, COLOR_CLR,
	COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED,
	COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED,
	COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED,
	COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED,
	COLOR_CLR, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_CLR,
	COLOR_CLR, COLOR_CLR, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_RED, COLOR_CLR, COLOR_CLR,
	COLOR_CLR, COLOR_CLR, COLOR_CLR, COLOR_CLR, COLOR_CLR, COLOR_CLR, COLOR_CLR, COLOR_CLR,
};
static const int appleBitmapSide = 8;

static unsigned int CreateTexture(const char* bitmap, int bitmapSideLen) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	const int level = 0;
	const int border = 0;
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, bitmapSideLen, bitmapSideLen, border, GL_RGB, GL_UNSIGNED_BYTE, bitmap);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

void r_InitGameRenderer(const char* shaderName) {
	const float w = 1.0 / (float)G_GRID_WIDTH;
	const float h = 1.0 / (float)G_GRID_HEIGHT;

	const float unit_verts[] = {
		0-0.5*w, 0-0.5*h, 0, 1,
		0-0.5*w, h-0.5*h, 0, 0,
		w-0.5*w, 0-0.5*h, 1, 1,
		w-0.5*w, 0-0.5*h, 1, 1,
		w-0.5*w, h-0.5*h, 1, 0,
		0-0.5*w, h-0.5*h, 0, 0
	};

	r_InitObjects(&gameRenderObjects, 6, shaderName);
	r_AddVertices(&gameRenderObjects, unit_verts, 6);

	gameSnakeBodyTexture = CreateTexture(snakeBodyBitmap, snakeBodyBitmapSide);
	gameSnakeHeadTexture = CreateTexture(snakeHeadBitmap, snakeHeadBitmapSide);
	gameAppleTexture = CreateTexture(appleBitmap, appleBitmapSide);

	//glUseProgram(gameRenderObjects.program);
	//glUniform1i(
	//	glGetUniformLocation(gameRendererObjects.program, "u_Texture"),
	//	0);

	loc_u_Offset = glGetUniformLocation(gameRenderObjects.program, "u_Offset");
	loc_u_Velocity = glGetUniformLocation(gameRenderObjects.program, "u_Velocity");
	loc_u_Color = glGetUniformLocation(gameRenderObjects.program, "u_Color");
}

void r_FreeGameRenderer(void) {
	r_FreeObjects(&gameRenderObjects);

	glDeleteTextures(1, &gameSnakeBodyTexture);
	glDeleteTextures(1, &gameSnakeHeadTexture);
	glDeleteTextures(1, &gameAppleTexture);
}

static void DrawSnakeNode(struct g_SnakeNode* ptr, float velX, float velY, float w, float h) {
	if (velX == 0 && velY == 0) {
		velX = 1;
	}

	glUniform2f(loc_u_Offset, (float)ptr->x * w + w * 0.5, (float)ptr->y * h + h * 0.5);
	glUniform2f(loc_u_Velocity, (float)velX, (float)velY);
	glDrawArrays(GL_TRIANGLES, 0, gameRenderObjects.vertices);

}

static void DrawSnake(float w, float h) {
	struct g_SnakeNode* ptr = gameState.head;
	if (ptr == NULL) {
		return;
	}
	
	struct g_SnakeNode* last_ptr = ptr;

	glBindTexture(GL_TEXTURE_2D, gameSnakeBodyTexture);

	int idx = gameState.applesAte;

	ptr = ptr->next;
	while (ptr != NULL) {
		const float velX = last_ptr->x - ptr->x;
		const float velY = last_ptr->y - ptr->y;
		const float shade = sqrtf((float)idx / (float)gameState.applesAte);
		glUniform3f(loc_u_Color, shade, shade, shade);
		DrawSnakeNode(ptr, velX, velY, w, h);

		idx--;
		last_ptr = ptr;
		ptr = ptr->next;
	}

	glBindTexture(GL_TEXTURE_2D, gameSnakeHeadTexture);

	glUniform3f(loc_u_Color, 1, 1, 1);
	DrawSnakeNode(gameState.head, gameState.velX, gameState.velY, w, h);
}

static void DrawApple(float w, float h) {
	glBindTexture(GL_TEXTURE_2D, gameAppleTexture);

	glUniform2f(loc_u_Offset, (float)gameState.appleX * w + w * 0.5, (float)gameState.appleY * h + h * 0.5);
	glUniform2f(loc_u_Velocity, 1, 0);
	glUniform3f(loc_u_Color, 1, 1, 1);
	glDrawArrays(GL_TRIANGLES, 0, gameRenderObjects.vertices);
}

void r_DrawGame(void) {
	const float w = 1.0 / (float)G_GRID_WIDTH;
	const float h = 1.0 / (float)G_GRID_HEIGHT;

	glUseProgram(gameRenderObjects.program);
	glBindVertexArray(gameRenderObjects.vao);

	DrawApple(w, h);
	DrawSnake(w, h);
}
