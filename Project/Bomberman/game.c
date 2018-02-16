#include "header.h"

#define SWITCHES ((PORTD >> 8) & 0xF) // get values of the 4 switches

///////////////////////////
// This file includes functions which update and control the game
///////////////////////////

/* Updates the LEDs to show each players lives */
void lives_led(void) {
    uint8_t l1 = (1 << p1.lives) - 1;
    uint8_t l2 = ~((1 << (8 - p2.lives))-1);
    update_led(l2 | l1);
}

/* Checks whether p1 or p2 has zero lives, goes to end screen if true */
void check_game_over(void) {
    if (p1.lives != 0 && p2.lives != 0)
        return;
    int i;
    for (i = 0; i < 8; ++i) {
        end[74  + i] = p1.lives == 0 ? two_big[0][i] : one_big[0][i];
        end[202 + i] = p1.lives == 0 ? two_big[1][i] : one_big[1][i];
    }
    state = END;
}

/* Draws a piller as a checkered pattern at (x,y) */
void draw_pillar(int x, int y) {
    x *= 4; y *= 4; // convert from internal- to pixel-coord
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            pixel(x+i, y+j, (i+j) & 1); // alternating pattern
        }
    }
}

/* Resets global variables to initial state */
void reset_vars(void) {
    int i, j;
    // reset pixels
    for (i = 0; i < 512; ++i) pixels[i] = 0;
    // place and draw pillars, reset position states
    for (i = 0; i < 32; ++i) {
        for (j = 0; j < 8; ++j) {
            if (i % 3 == 1 && j % 3 == 1) {
                position_state[i][j] = OCCUPIED;
                draw_pillar(i, j);
            }
            else position_state[i][j] = FREE;
        }
    }
    // reset all bombs as inactive
    for (i = 0; i < 2; ++i) {
        for (j = 0; j < MAX_BOMBS; ++j) {
            Bomb *b = &(bombs[i][j]);
            b->x = -1; b->y = -1; b->frames_left = 0;
            b->up = 0; b->down = 0; b->left = 0; b->right = 0;
        }
    }
    // reset players
    p1.x = 30; p1.y = 6; p1.id = 0; p1.has_placed = 0; p1.dir = LEFT;
    p2.x = 2;  p2.y = 2; p2.id = 1; p2.has_placed = 0; p2.dir = RIGHT;
    draw_player(&p1); draw_player(&p2);
}

/* Resets the state of the game to initial state */
void reset(void) {
    update_display(pixels); update_display(pixels);
    delay(3000000); // makes death more visible

    lives_led();
    check_game_over();
    reset_vars();

    update_display(pixels);
}

/* Decrement frames left for every active bomb and explode them etc */
void update_bombs(void) {
    if (p1.has_placed) place_bomb(&p1);
    if (p2.has_placed) place_bomb(&p2);
    p1.has_placed = 0; p2.has_placed = 0;

    int i,j;
    for (i = 0; i < 2; ++i) {
        for (j = 0; j < MAX_BOMBS; ++j) {
            Bomb *b = &(bombs[i][j]);
            if (b->frames_left == 0)
                continue;
            draw_bomb(b);
            if (b->frames_left == EXPLODE)    explode(b);
            if (b->frames_left == 1) remove_explosion(b);
            --b->frames_left;
        }
    }
}

/* Main update function for the game */
void run_game(void) {
    p1.lives = START_LIVES; p2.lives = START_LIVES;
    reset();

    int timer_delay = 0, step = 0;
    int last_sw1 = SWITCHES & 3, last_sw2 = SWITCHES >> 2;
    while(1) {
        /* Poll input */
        if (BUTTON(1)) {
            state = START;
            delay(3000000); // otherwise we instantly return to game-state
        }
        if (state != GAME) return;

        if (BUTTON(0)) p1.has_placed = 1;
        if (BUTTON(3)) p2.has_placed = 1;

        int sw1 = SWITCHES & 3, sw2 = SWITCHES >> 2;
        /* Poll input */

        if (!TIMER) continue;
        RESET_TIMER;
        if(timer_delay++ < TIMER_DELAY) continue;
        timer_delay = 0;

        /* Step animation */
        if (step++ < 3) {
            update_display(pixels);
            step_forw(&p1, step);
            step_forw(&p2, step);
            update_display(pixels);
            continue;
        }
        step = 0;

        /* Update game */
        update_display(pixels);
        update_bombs();
        move(&p1); move(&p2);
        update_display(pixels);
        if (last_sw1 != sw1) change_dir(&p1, &last_sw1, sw1);
        if (last_sw2 != sw2) change_dir(&p2, &last_sw2, sw2);
    }
}
