#include "m_menu.h"

#include <stdio.h>
#include <string.h>

#include "g_game.h"

struct m_MenuState menuState;

void m_Init() {
	memset(&menuState, 0, sizeof(menuState));
}

void m_PressButton(int button) {
	if (gameState.lost && button == M_BUTTON_START) {
		button = M_BUTTON_RESTART;
	}

	switch (button) {
		case M_BUTTON_START:
			menuState.open = 0;
			break;
		case M_BUTTON_RESTART:
			menuState.open = 0;
			g_DestroySnake();
			g_Init();
			break;
		case M_BUTTON_QUIT:
			gameState.active = 0;
			break;
	}
}

int m_ProcessEvent(SDL_Event* event) {
	// Able to open menu if not lost game
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE && !gameState.lost) {
		menuState.buttonActive = 0;
		menuState.open = !menuState.open;
		return 1;
	}

	if (menuState.open == 0) {
		return 0;
	}

	if (event->type != SDL_KEYDOWN) {
		return 1;
	}

	switch (event->key.keysym.sym) {
		case SDLK_LEFT:
			menuState.buttonActive--;
			break;
		case SDLK_RIGHT:
			menuState.buttonActive++;
			break;
		case SDLK_RETURN:
			m_PressButton(menuState.buttonActive);
			break;
		default:
			break;
	}

	if (menuState.buttonActive < 0) { menuState.buttonActive = 0; }
	if (menuState.buttonActive >= M_NUM_BUTTONS) { menuState.buttonActive = M_NUM_BUTTONS - 1; }

	return 1;
}
