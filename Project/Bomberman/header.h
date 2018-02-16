#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>   /* For uint8_t */
#include <pic32mx.h>  /* System-specific addresses etc */

/* Game parameters */
#define START_LIVES      1  // how many lives players start with
#define TIMER_DELAY      5  // affects the speed of the game

#define MAX_BOMBS        3  // max number of bombs a player can have active
#define MAX_BOMB_LENGTH  7  // max length of an explosion
#define BOMB_DURATION   15  // number of frames bombs are active
#define EXPLODE          5  // number of frames explosions are active

/* For readability */
#define OCCUPIED 1  // position state is occupied
#define DEATH    2  // position state is deadly for the player
#define FREE     0  // position state is free

/* Macros */
#define BUTTON(x)   (getbtns() >> x) & 1   // if btn x is down
#define TIMER       ((IFS(0) >> 8) & 0x1)  // 8th bit is interrupt flag for timer 2
#define RESET_TIMER IFS(0) &= ~(0x1 << 8)  // set 8th bit to 0

/* global */
enum Direction {UP, DOWN, LEFT, RIGHT};
enum GameState {START, GAME, END};

typedef struct {
    int x, y;
    int up, down, left, right;
    int frames_left;
} Bomb;

typedef struct {
    int x, y;
    int id, has_placed, lives;
    enum Direction dir;
} Player;

/* main.c */
extern uint8_t pixels[512];
extern uint8_t position_state[32][8];
extern Bomb bombs[2][MAX_BOMBS];
extern Player p1, p2;
extern enum GameState state;

/* data.c */
extern uint8_t start[512];
extern uint8_t end[512];
extern uint8_t btn2[23];
extern uint8_t two_big[2][8];
extern uint8_t one_big[2][8];

#endif /* HEADER_H */
