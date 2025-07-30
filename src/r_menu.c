#include "r_menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

#include "m_menu.h"
#include "r_main.h"

#include "gimp_image_menu.h"

static struct r_RenderObjects menuRenderObjects;
static unsigned int menuTexture;
static int loc_u_PositionOff;
static int loc_u_UvOff;


static unsigned int CreateTexture(const char* bitmap, int width, int height) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	const int level = 0;
	const int border = 0;
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_RGB, GL_UNSIGNED_BYTE, bitmap);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

void r_InitMenuRenderer(const char* shaderName) {
	const float HALF = 0.5f;
	const float THIRD = 1.0f / 3.0f;
	const float DIST = 1.0f / 3.0f;
	const float vertices[] = {
		    -HALF, -DIST*0.5f,     0    , HALF,
		    -HALF, +DIST*0.5f,     0    , 0,
		DIST-HALF, +DIST*0.5f,     THIRD, 0,
		    -HALF, -DIST*0.5f,     0    , HALF,
		DIST-HALF, -DIST*0.5f,     THIRD, HALF,
		DIST-HALF, +DIST*0.5f,     THIRD, 0
	};

	r_InitObjects(&menuRenderObjects, 6, shaderName);
	r_AddVertices(&menuRenderObjects, vertices, 6);

	menuTexture = CreateTexture(gimp_image_menu.pixel_data, gimp_image_menu.width, gimp_image_menu.height);

	loc_u_PositionOff = glGetUniformLocation(menuRenderObjects.program, "u_PositionOff");
	loc_u_UvOff       = glGetUniformLocation(menuRenderObjects.program, "u_UvOff");
}

void r_FreeMenuRenderer(void) {
	r_FreeObjects(&menuRenderObjects);
}

void r_DrawMenu(void) {
	glBindVertexArray(menuRenderObjects.vao);
	glUseProgram(menuRenderObjects.program);
	glBindTexture(GL_TEXTURE_2D, menuTexture);

	for (int i = 0; i < 3; i++) {
		float offx = (float)i / 3.0f;
		float offy = menuState.buttonActive == i ? 0.5f : 0.0f;
		glUniform1f(loc_u_PositionOff, offx);
		glUniform2f(loc_u_UvOff      , offx, offy);
		glDrawArrays(GL_TRIANGLES, 0, menuRenderObjects.vertices);
	}
}
