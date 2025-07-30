#pragma once

#include "r_main.h"
#include "g_game.h"

void r_InitGameRenderer(const char* shaderName);
void r_FreeGameRenderer(void);
void r_DrawGame(void);
