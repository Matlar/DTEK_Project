#include "header.h"

///////////////////////////
// This file includes functions related to the player struct
///////////////////////////

/* Decrements players lives, resets game */
void death(Player *p) {
    --p->lives;
    reset();
}

/* Places a bomb at players x,y if there are inactive bombs */
void place_bomb(Player *p) {
    int i, id = p->id;
    for (i = 0; i < MAX_BOMBS; ++i) {
        Bomb *b = &(bombs[id][i]);
        if (b->frames_left != 0)
            continue;
        b->frames_left = BOMB_DURATION;
        b->x = p->x; b->y = p->y;
        position_state[p->x][p->y] = OCCUPIED;
        return;
    }
}

/* Returns true if the player is trying to walk into
   a wall, pillar, or another player */
int cannot_move(Player *p) {
    int x = p->x, y = p->y;
    switch (p->dir) {
    case UP:
        if (y == 0  || position_state[x][y-1] == OCCUPIED)
            return 1;
        break;
    case DOWN:
        if (y == 7  || position_state[x][y+1] == OCCUPIED)
            return 1;
        break;
    case LEFT:
        if (x == 0  || position_state[x-1][y] == OCCUPIED)
            return 1;
        break;
    case RIGHT:
        if (x == 31 || position_state[x+1][y] == OCCUPIED)
            return 1;
        break;
    }
    return 0;
}

/* Move the player in it's current direction
   Erases it's last position and redraws
   Updates the list of position_state */
void move(Player *p) {
    if (cannot_move(p))
        return;
    int x = p->x, y = p->y;
    position_state[x][y] = FREE;
    delete_pixels(x,y);
    switch (p->dir) {
        case UP:    --y; break;
        case DOWN:  ++y; break;
        case LEFT:  --x; break;
        case RIGHT: ++x; break;
    }
    if (position_state[x][y] == DEATH) {
        // we walked into a bomb
        death(p);
        return;
    }
    p->x = x; p->y = y;
    position_state[x][y] = OCCUPIED;
    draw_player(p);
}

/* Draws the player as a 4x4 filled in square */
void draw_player(Player *p) {
    int x = p->x * 4, y = p->y * 4; // convert from internal- to pixel-coord
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; j++) {
            pixel(x+i, y+j, 1);
        }
    }
}

/* Draws the player at it's position, with a given pixel-offset
   in the direction the player is facing */
void step_forw(Player *p, int offset) {
    if (cannot_move(p))
        return;
    delete_pixels(p->x, p->y);
    int x_off = 0, y_off = 0;
    switch (p->dir) {
        case UP:    y_off = -offset; break;
        case DOWN:  y_off =  offset; break;
        case LEFT:  x_off = -offset; break;
        case RIGHT: x_off =  offset; break;
    }

    int y = p->y * 4;
    int x = p->x * 4;
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; j++) {
            pixel(x+i+x_off, y+j+y_off, 1);
        }
    }
}

/* Changes the player direction based on what switch changed */
void change_dir(Player *p, int *last_sw, int sw) {
    if ((*last_sw ^ sw) == 1)
        p->dir = (sw & 1) == 1 ? UP : DOWN;
    else
        p->dir = (sw >> 1) == 1 ? LEFT : RIGHT;
    *last_sw = sw;
}
