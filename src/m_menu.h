#pragma once

#include <SDL2/SDL.h>

#define M_BUTTON_START   0
#define M_BUTTON_RESTART 1
#define M_BUTTON_QUIT    2
#define M_NUM_BUTTONS    3

struct m_MenuState {
	int open;
	int buttonActive;
};

extern struct m_MenuState menuState;

void m_Init();
void m_PressButton(int button);
int m_ProcessEvent(SDL_Event* event);
