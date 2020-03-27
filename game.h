#ifndef GAME_H
#define GAME_H

#include "gba.h"

                    /* TODO: */

            // Create any necessary structs //


/*
* For example, for a Snake game, one could be:
*
* typedef struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* } Snake;
*
*
* 
*
*
* Example of a struct to hold state machine data:
* 
* typedef struct state {
*   int currentState;
*   int nextState;
* } State
*
*/


typedef struct {
	int x;
	int y;
	int alive;
	int direction;
} Alien1;

typedef struct {
	int x;
	int y;
	int alive;
	int direction;
	int startX;
	int targetX;
	int targetY;
} Alien2;

typedef struct {
	int x;
	int y;
	int alive;
	int direction;
} Alien3;

typedef struct {
	int x; //positions
	int y;
	int health; //health
	int alive;
} Ship;

typedef struct bullet {
	int x;
	int y;
	int alive;
} Bullet;

typedef struct {
	int points;
	int gameOver;
	Alien1 alien1;
	Alien2 alien2;
	Alien3 alien3;
	Ship ship;
	Bullet bullet;
} GameState;

void createGameState(GameState *gameState);
GameState updateGameState(GameState *gameState, u32 previousButtons, u32 currentButtons);
void drawGameState(GameState *gameState);
void checkCollision(GameState *gameState);
void generateAlien(GameState *gameState);
void showPoints(GameState *gameState);
#endif
