#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
/* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
#include "images/garbage.h"
#include "images/titlescreen.h"
#include "images/ship.h"
#include "images/alien1.h"
#include "images/alien2.h"
#include "images/alien3.h"
#include "images/gameover.h"
// #include "images/garbage.h"

/* TODO: */
// Add any additional states you need for your app.
typedef enum {
  START,
  WAITFORPLAY,
  INITGAME,
  PLAY,
  END,
} GBAState;

int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;
  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial game state
  GBAState state = START;

  GameState currState, nextState;

  while (1) {
    currentButtons = BUTTONS;  // Load the current state of the buttons

    /* TODO: */
    // Manipulate the state machine below as needed //
    // NOTE: Call waitForVBlank() before you draw

    switch (state) {
      case START:
      	waitForVBlank();
      	drawFullScreenImageDMA(titlescreen);
        // state = ?
        state = WAITFORPLAY;
        break;
      case WAITFORPLAY:
      	if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) {
      		state = INITGAME;
      	}
      	break;
      case INITGAME:
      	waitForVBlank();
      	fillScreenDMA(BLACK);
      	createGameState(&currState);
    	drawGameState(&currState);
    	state = PLAY;
        // state = ?
        break;
      case PLAY:
      	nextState = updateGameState(&currState, previousButtons, currentButtons);
      	if (nextState.gameOver == 1) {
      		state = END;
      	}
      	waitForVBlank();
      	fillScreenDMA(BLACK);
      	drawGameState(&nextState);
      	currState = nextState;
      	if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
      		state = START;
      	}
      	break;
      case END:
      	waitForVBlank();
      	drawFullScreenImageDMA(gameover);
      	waitForVBlank();
      	showPoints(&currState);
      	if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
      		state = START;
      	}
        // state = ?
        break;
    }
    
    previousButtons = currentButtons;  // Store the current state of the buttons
  }

  UNUSED(previousButtons);  // You can remove this once previousButtons is used

  return 0;
}

void showPoints(GameState *gameState) {
	int p = gameState->points;
	char buffer[10];
	drawString(90, 80, "Points earned:", RED);
	sprintf(buffer, "%d", p);
	drawString(105, 80, buffer, RED);
}

void generateAlien(GameState *gameState) {
	int randDir = rand() % 2 + 1; //1-2
	int randNum = rand() % 4 + 1; //1-4
	if (randNum == 1) {
		Alien1 *a1 = &(gameState->alien1);
		a1->x = WIDTH/2 - ALIEN1_WIDTH/2;
		a1->y = 5;
		a1->alive = 1;
		if (randDir == 2) {
			a1->direction = -1;
		} else {
			a1->direction = 1;
		}
		Alien2 *a2 = &(gameState->alien2);
		// a2->x = -1;
		// a2->y = -1;
		a2->alive = 0;
		Alien3 *a3 = &(gameState->alien3);
		// a3->x = -1;
		// a3->y = -1;
		a3->alive = 0;
	} else if (randNum == 2 || randNum == 3) {
		Ship *s = &(gameState->ship);
		Alien1 *a1 = &(gameState->alien1);
		// a1->x = -1;
		// a1->y = -1;
		a1->alive = 0;
		Alien2 *a2 = &(gameState->alien2);
		a2->x = WIDTH/2 - ALIEN2_WIDTH/2;
		a2->y = 5;
		a2->startX = WIDTH/2 - ALIEN2_WIDTH/2;
		a2->alive = 1;
		a2->targetX = s->x + 1;
		a2->targetY = s->y + 1;
		Alien3 *a3 = &(gameState->alien3);
		// a3->x = -1;
		// a3->y = -1;
		a3->alive = 0;
	} else {
		Alien1 *a1 = &(gameState->alien1);
		// a1->x = -1;
		// a1->y = -1;
		a1->alive = 0;
		Alien2 *a2 = &(gameState->alien2);
		// a2->x = -1;
		// a2->y = -1;
		a2->alive = 0;
		Alien3 *a3 = &(gameState->alien3);
		a3->x = WIDTH/2 - ALIEN3_WIDTH/2;
		a3->y = 5;
		a3->alive = 1;
		if (randDir == 2) {
			a3->direction = -1;
		} else {
			a3->direction = 1;
		}
	}
}

void createGameState(GameState *gameState) {
	gameState->points = 0;
	gameState->gameOver = 0;
	Ship *s = &(gameState->ship);
	s->x = 0;
	s->y = HEIGHT - SHIP_HEIGHT;
	s->health = 3;
	s->alive = 1;
	generateAlien(gameState);
	Bullet *b = &(gameState->bullet);
	b->alive = 0;
}

GameState updateGameState(GameState *gameState, u32 previousButtons, u32 currentButtons) {
	GameState *next = gameState;
	Alien1 *a1 = &(next->alien1);
	Alien2 *a2 = &(next->alien2);
	Alien3 *a3 = &(next->alien3);
	Ship *s = &(next->ship);
	Bullet *b = &(next->bullet);	
	if (a1->alive == 0 && a2->alive == 0 && a3->alive == 0) {
		generateAlien(next);
	}

	if (b->alive == 0 && KEY_JUST_PRESSED(BUTTON_B, previousButtons, currentButtons)) {
		b->x = s->x + SHIP_WIDTH/2;
		b->y = s->y;
		b->alive = 1;
	} else if (b->alive == 1) {
		b->y = b->y - 5;
		if (b->y < 0) {
			b->alive = 0;
		}
	}

	if (s->alive == 1 && KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
		if (s->x + 3 > WIDTH - SHIP_WIDTH) {
			s->x = WIDTH - SHIP_WIDTH;
		} else {
			s->x = s->x + 3;
		}
	}
	if (s->alive == 1 && KEY_DOWN(BUTTON_LEFT, currentButtons)) {
		if (s->x - 3 < 0) {
			s->x = 0;
		} else {
			s->x = s->x - 3;
		}
	}

	if (a1->alive == 1) {
		if (a1->direction > 0) {
			if (a1->x + 4 > WIDTH - ALIEN1_WIDTH) {
				a1->x = WIDTH - ALIEN1_WIDTH;
				a1->direction = -1;
			} else {
				a1->x = a1->x + 4;
			}
		} else {
			if (a1->x - 4 < 0) {
				a1->x = 0;
				a1->direction = 1;
			} else {
				a1->x = a1->x - 4;
			}
		}
	}

	if (a2->alive == 1) {
		if (a2->x + 2 > WIDTH - ALIEN2_WIDTH) {
			a2->x = WIDTH - ALIEN2_WIDTH;
			a2->alive = 0;
		} else if (a2->x - 2 < 0) {
			a2->x = 0;
			a2->alive = 0;
		} else if (a2->y + 2 > HEIGHT - ALIEN2_HEIGHT) {
			a2->y = HEIGHT - ALIEN2_HEIGHT;
			a2->alive = 0;
		} else { //it is in valid spot, chase after player
		 	int diff = a2->x - a2->targetX;
		 	if (diff < 0) {
		 		a2->x = a2->x + 2;
		 	} else if (diff > 0) {
		 		a2->x = a2->x - 2;
		 	}
		 	a2->y = a2->y + 3;
		}
	}

	if (a3->alive == 1) {
		if (a3->direction > 0) {
			if (a3->x + 4 > WIDTH - ALIEN3_WIDTH) {
				a3->x = WIDTH - ALIEN3_WIDTH;
				a3->direction = -1;
			} else {
				a3->x = a3->x + 4;
			}
		} else {
			if (a3->x - 4 < 0) {
				a3->x = 0;
				a3->direction = 1;
			} else {
				a3->x = a3->x - 4;
			}
		}
	}


	checkCollision(next);
	if (s->health == 0) {
		next->gameOver = 1;
	}
	return *next;
}

void drawGameState(GameState *gameState) {
	//draw ship, bullet, and alien
	Alien1 *a1 = &(gameState->alien1);
	Alien2 *a2 = &(gameState->alien2);
	Alien3 *a3 = &(gameState->alien3);
	if (a1->alive == 1) {
		drawImageDMA(a1->y, a1->x, ALIEN1_WIDTH, ALIEN1_HEIGHT, alien1);
	}
	if (a2->alive == 1) {
		drawImageDMA(a2->y, a2->x, ALIEN2_WIDTH, ALIEN2_HEIGHT, alien2);
	}
	if (a3->alive == 1) {
		drawImageDMA(a3->y, a3->x, ALIEN3_WIDTH, ALIEN3_HEIGHT, alien3);
	}
	Bullet *b = &(gameState->bullet);
	if (b->alive == 1) {
		drawRectDMA(b->y, b->x, 2, 4, RED);
	}
	Ship *s = &(gameState->ship);
	if (s->alive == 1) {
		drawImageDMA(s->y, s->x, SHIP_WIDTH, SHIP_HEIGHT, ship);
	}
}

void checkCollision(GameState *gameState) {
	Alien1 *a1 = &(gameState->alien1);
	Alien2 *a2 = &(gameState->alien2);
	Alien3 *a3 = &(gameState->alien3);
	Ship *s = &(gameState->ship);
	Bullet *b = &(gameState->bullet);
	int bulletLeft = b->x;
	int bulletRight = b->x + 2;
	int bulletTop = b->y;
	int bulletBottom = b->y + 4;
	if (a1->alive == 1 && b->alive == 1) {
		int alienLeft = a1->x;
		int alienRight = a1->x + ALIEN1_WIDTH;
		int alienTop = a1->y;
		int alienBottom = a1->y + ALIEN1_HEIGHT;
		if (alienRight > bulletLeft && alienLeft < bulletRight && alienTop < bulletBottom && alienBottom > bulletTop) {
			b->alive = 0;
			a1->alive = 0;
			gameState->points = gameState->points + 10;
		}
	}
	if (a2->alive == 1) {
		int shipLeft = s->x;
		int shipRight = s->x + SHIP_WIDTH;
		int shipTop = s->y;
		int shipBottom = s->y + SHIP_HEIGHT;
		int alienLeft = a2->x;
		int alienRight = a2->x + ALIEN2_WIDTH;
		int alienTop = a2->y;
		int alienBottom = a2->y + ALIEN2_HEIGHT;
		if (b->alive == 1) {
			if (alienRight > bulletLeft && alienLeft < bulletRight && alienTop < bulletBottom && alienBottom > bulletTop) {
				b->alive = 0;
				a2->alive = 0;
				gameState->points = gameState->points + 10;
			}
		}
		if (alienRight > shipLeft && alienLeft < shipRight && alienTop < shipBottom && alienBottom > shipTop) {
			s->health = s->health - 1;
			a2->alive = 0;
		}
	}
	if (a3->alive == 1 && b->alive == 1) {
		int alienLeft = a3->x;
		int alienRight = a3->x + ALIEN1_WIDTH;
		int alienTop = a3->y;
		int alienBottom = a3->y + ALIEN1_HEIGHT;
		if (alienRight > bulletLeft && alienLeft < bulletRight && alienTop < bulletBottom && alienBottom > bulletTop) {
			b->alive = 0;
			a3->alive = 0;
			gameState->points = gameState->points + 10;
		}
	}
}

