#include "header.h"

///////////////////////////
// This file includes functions related to the bomb struct
///////////////////////////

/* Draws a bomb as a 4x4 'x' at given (x,y) */
void draw_bomb(Bomb *b) {
    int x = b->x * 4, y = b->y * 4; // convert from internal- to pixel-coord
    int i;
    for (i = 0; i < 4; ++i) {
        pixel(x+i, y+i,  1); // draws '\'
        pixel(x+i, y+3-i,1); // draws '/'
    }
}

/* Draw explosion at (x,y), vert indicates if to draw horizontal or vertical */
void draw_explosion(int x, int y, int vert) {
    x *= 4; y *= 4; // convert from internal- to pixel-coord
    int i;
    if (vert) {
        for (i = 0; i < 4; ++i) {
            pixel(x+i,y+1,1);
            pixel(x+i,y+2,1);
        }
    } else {
        for (i = 0; i < 4; ++i) {
            pixel(x+1,y+i,1);
            pixel(x+2,y+i,1);
        }
    }
}

/* Check whether an explosion hit something */
int check_hit(int x, int y) {
    if (position_state[x][y] != OCCUPIED) return 0;
    if (p1.x == x && p1.y == y) return 1;
    if (p2.x == x && p2.y == y) return 2;

    // we hit a pillar, destroy it
    delete_pixels(x,y);
    position_state[x][y] = FREE;
    return 0;
}

/* Explodes in all directions until stopped by player/pillar/wall etc
   or until it's hit the MAX_BOMB_LENGTH */
void explode(Bomb *b) {
    int x = b->x, y = b->y, max = MAX_BOMB_LENGTH, has_died = 0, tmp;
    // check that the player isn't standing on the bomb
    if ((tmp = check_hit(x,y)) != 0) has_died = tmp;
    position_state[x][y] = DEATH;

    // Explode LEFT
    while(x > 0  && position_state[--x][y] != OCCUPIED && --max > 0) {
        position_state[x][y] = DEATH;
        draw_explosion(x, y, 1);
    }
    b->left = MAX_BOMB_LENGTH - max;
    if ((tmp = check_hit(x,y)) != 0) has_died = tmp; // we hit a player!

    x = b->x;
    max = MAX_BOMB_LENGTH;
    // Explode RIGHT
    while(x < 31 && position_state[++x][y] != OCCUPIED && --max > 0) {
        position_state[x][y] = DEATH;
        draw_explosion(x, y, 1);
    }
    b->right = MAX_BOMB_LENGTH - max;
    if ((tmp = check_hit(x,y)) != 0) has_died = tmp; // we hit a player!

    x = b->x;
    max = MAX_BOMB_LENGTH;
    // Explode UP
    while(y > 0  && position_state[x][--y] != OCCUPIED && --max > 0) {
        position_state[x][y] = DEATH;
        draw_explosion(x, y, 0);
    }
    b->up = MAX_BOMB_LENGTH - max;
    if ((tmp = check_hit(x,y)) != 0) has_died = tmp; // we hit a player!

    y = b->y;
    max = MAX_BOMB_LENGTH;
    // Explode DOWN
    while(y < 7  && position_state[x][++y] != OCCUPIED && --max > 0) {
        position_state[x][y] = DEATH;
        draw_explosion(x, y, 0);
    }
    b->down = MAX_BOMB_LENGTH - max;
    if ((tmp = check_hit(x,y)) != 0) has_died = tmp; // we hit a player!

    if(has_died == 1) death(&p1);
    if(has_died == 2) death(&p2);
}

/* Remove the explosion, i.e pixels and position states */
void remove_explosion(Bomb *b) {
    int x = b->x, y = b->y;
    int left = b->left, right = b->right, up = b->up, down = b->down;
    position_state[x][y] = FREE;
    delete_pixels(x,y);

    while(--left >= 0) {
        delete_pixels(--x,y);
        position_state[x][y] = FREE;
    }
    x = b->x;

    while(--right >= 0) {
        delete_pixels(++x,y);
        position_state[x][y] = FREE;
    }
    x = b->x;

    while(--up >= 0) {
        delete_pixels(x,--y);
        position_state[x][y] = FREE;
    }
    y = b->y;

    while(--down >= 0) {
        delete_pixels(x,++y);
        position_state[x][y] = FREE;
    }
}
