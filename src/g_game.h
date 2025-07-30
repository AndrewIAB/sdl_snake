#pragma once

#include <SDL2/SDL_events.h>

#define G_GRID_WIDTH 16
#define G_GRID_HEIGHT 16

#define G_SNAKE_MOVE_TIMER 0.2f
#define G_SNAKE_TURN_TIMER 0.01f

struct g_SnakeNode {
	int x, y;
	struct g_SnakeNode* next;
};

struct g_GameState {
	float moveTimer;
	int applesAte;
	struct g_SnakeNode* head;
	int velX, velY;
	int appleX, appleY;

	int active;
	int lost;
	int hasMoved;
};

extern struct g_GameState gameState;

void g_Init(void);
void g_DestroySnake(void);
void g_ProcessKey(int key);
void g_ProcessEvent(SDL_Event* event);
int g_SnakeSelfCollision(void);
int g_SnakeWallCollision(void);
int g_SnakeAppleCollision(void);
void g_RandomApple(void);
void g_MoveSnake(void);
void g_GrowSnake(void);
void g_Tick(float deltaTime);
