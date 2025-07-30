#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "r_game.h"
#include "r_lighting.h"
#include "r_menu.h"
#include "g_game.h"
#include "m_menu.h"

#define INIT_WINDOW_WIDTH 800
#define INIT_WINDOW_HEIGHT 800

static int ProcessFrameEvents(void) {
	if (gameState.active == 0) {
		return 0;
	}

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			g_ProcessEvent(&event);
			return 0;
		}

		if (!m_ProcessEvent(&event)) {
			g_ProcessEvent(&event);
		}
	}

	return 1;
}

static void RenderFrame(SDL_Window* window) {
	if (menuState.open) {
		glDrawBuffer(GL_FRONT);
		r_DrawMenu();
		glFlush();
	} else {
		glDrawBuffer(GL_BACK);
		r_DrawLighting();
		r_DrawGame();
		SDL_GL_SwapWindow(window);
	}
}

int main(void) {
	srand(time(NULL));

	SDL_Window* window;
	SDL_GLContext window_context;
	unsigned int window_flags = 0;
	window_flags |= SDL_WINDOW_OPENGL;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Game", 0, 0, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, window_flags);
	window_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, window_context);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	r_InitGameRenderer("snake");
	r_InitLighting("lights");
	r_InitMenuRenderer("menu");
	g_Init();
	m_Init();

	menuState.open = 1;

	int running = 1;

	const double invCounterInterval = 1.0 / (double)SDL_GetPerformanceFrequency();
	uint64_t curr_counter = SDL_GetPerformanceCounter();
	uint64_t last_counter;

	SDL_Delay(10);

	while (running) {
		last_counter = curr_counter;
		curr_counter = SDL_GetPerformanceCounter();

		double deltaTime = (double)(curr_counter - last_counter) * invCounterInterval;

		if (!menuState.open) {
			g_Tick((float)deltaTime);
		}

		running = ProcessFrameEvents();

		if (gameState.hasMoved || menuState.open) {
			RenderFrame(window);
		}
	}

	g_DestroySnake();
	r_FreeGameRenderer();
	r_FreeLighting();
	r_FreeMenuRenderer();

	return 0;
}
