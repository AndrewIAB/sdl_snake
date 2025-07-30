#include "g_game.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "m_menu.h"

struct g_GameState gameState;

void g_Init(void) {
	memset(&gameState, 0, sizeof(gameState));
	gameState.head = malloc(sizeof(struct g_SnakeNode));
	memset(gameState.head, 0, sizeof(struct g_SnakeNode));

	gameState.active = 1;

	g_RandomApple();
}

void g_DestroySnake(void) {
	struct g_SnakeNode* ptr = gameState.head;
	while (ptr != NULL) {
		struct g_SnakeNode* next_ptr = ptr->next;
		free(ptr);
		ptr = next_ptr;
	}
}

void g_ProcessKey(int key) {
	if (gameState.head == NULL) {
		return;
	}

	int newVelX = gameState.velX;
	int newVelY = gameState.velY;

	if (key == SDLK_UP) {
		newVelX = 0;
		newVelY = 1;
	} else
	if (key == SDLK_DOWN) {
		newVelX = 0;
		newVelY = -1;
	} else
	if (key == SDLK_LEFT) {
		newVelX = -1;
		newVelY = 0;
	} else
	if (key == SDLK_RIGHT) {
		newVelX = 1;
		newVelY = 0;
	} else {
		return;
	}

	if (newVelX == gameState.velX && newVelY == gameState.velY) {
		return;
	}
	//if (newVelX == -gameState.velX && -newVelY == gameState.velY) {
	//	return;
	//}
	if (gameState.head->next != NULL &&
		newVelX + gameState.head->x == gameState.head->next->x &&
		newVelY + gameState.head->y == gameState.head->next->y)
	{
		return;
	}

	gameState.velX = newVelX;
	gameState.velY = newVelY;

	if (gameState.moveTimer >= G_SNAKE_TURN_TIMER) {
		gameState.moveTimer = G_SNAKE_MOVE_TIMER;
		g_Tick(0);
	}
}

void g_ProcessEvent(SDL_Event* event) {
	if (event->type == SDL_QUIT) {
		g_DestroySnake();
		return;
	}

	if (event->type == SDL_KEYDOWN) {
		g_ProcessKey(event->key.keysym.sym);
	}
}

int g_SnakeSelfCollision(void) {
	struct g_SnakeNode* ptr = gameState.head->next;
	while (ptr != NULL) {
		if (ptr->x == gameState.head->x && ptr->y == gameState.head->y) {
			return 1;
		}
		ptr = ptr->next;
	}
	return 0;
}

int g_SnakeWallCollision(void) {
	const int x = gameState.head->x;
	const int y = gameState.head->y;

	return (x < 0 || x >= G_GRID_WIDTH) || (y < 0 || y >= G_GRID_HEIGHT);
}

int g_SnakeAppleCollision(void) {
	const int x = gameState.head->x;
	const int y = gameState.head->y;

	return (x == gameState.appleX && y == gameState.appleY);
}

static int IsValidApple() {
	struct g_SnakeNode* ptr = gameState.head;
	while (ptr != NULL) {
		if (ptr->x == gameState.appleX && ptr->y == gameState.appleY) {
			return 0;
		}
		ptr = ptr->next;
	}
	return 1;
}

void g_RandomApple() {
	while (IsValidApple() == 0) {
		gameState.appleX = rand() % G_GRID_WIDTH;
		gameState.appleY = rand() % G_GRID_HEIGHT;
	}
}

void g_MoveSnake() {
	struct g_SnakeNode* ptr = gameState.head;
	if (ptr == NULL) {
		return;
	}

	gameState.hasMoved = 1;

	int lastX = ptr->x + gameState.velX;
	int lastY = ptr->y + gameState.velY;

	while (ptr != NULL) {
		int nextX = lastX;
		int nextY = lastY;
		lastX = ptr->x;
		lastY = ptr->y;
		ptr->x = nextX;
		ptr->y = nextY;

		ptr = ptr->next;
	}
}

void g_GrowSnake(void) {
	struct g_SnakeNode* ptr = gameState.head;
	if (ptr == NULL) {
		return;
	}

	int velX = gameState.velX;
	int velY = gameState.velY;

	while (ptr->next != NULL) {
		velX = ptr->x - ptr->next->x;
		velY = ptr->y - ptr->next->y;
		ptr = ptr->next;
	}

	ptr->next = malloc(sizeof(struct g_SnakeNode));
	memset(ptr->next, 0, sizeof(struct g_SnakeNode));
	ptr->next->x = ptr->x - velX;
	ptr->next->y = ptr->y - velY;
}

void g_Tick(float deltaTime) {
	if (g_SnakeSelfCollision() || g_SnakeWallCollision()) {
		gameState.lost = 1;
		menuState.open = 1;
		return;
	}

	gameState.hasMoved = 0;

	if (g_SnakeAppleCollision()) {
		g_RandomApple();
		g_GrowSnake();
		gameState.applesAte++;
		gameState.moveTimer = 0;
	}

	gameState.moveTimer += deltaTime;
	if (gameState.moveTimer >= G_SNAKE_MOVE_TIMER) {
		g_MoveSnake();
		gameState.moveTimer = 0;
	}
}
