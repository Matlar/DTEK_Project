/* Not correct C-code! This is a first prototyping
   of how we want to organize our program */

struct Player {
    int x;
    int y;
    int alive;
    int direction;
    int current_bombs;
}

struct Bomb {
    int x;
    int y;
    int frames_left;
}

int on_start_screen = 1;

Bomb player1_bombs[3];
Bomb player2_bombs[3];

uint8_t pixels[4 * 128]; // 4 bytes per column, 128 columns

Player play1 = {5,7,1};
Player play2 = {0,0,1};

update() {
    // will be on a timer
    update_positions();
    update_bombs();
    redraw();
    update_screen();
}

update_positions() {
    // get switch-states
    // preform checks, i.e don't walk into walls, pillars, or the other Player
    // otherwise update x,y of players
}

update_bombs() {
    // decrement frames left of every bomb, trigger explosions if 0 etc
}

redraw() {
    // update the pixel array based on the bombs, players, pillars
}

update_screen() {
    // take the pixel array and display it on the screen
    push_to_screen(pixels);
}

poll_buttons() {
    // check if the buttons are pressed. Place a bomb if they are etc
    // maybe check for a reset button
}

reset() {
    // reset all global variables to initial state
}

main() {
    while(1) {
        if (on_start_screen) {
            reset();
            // display start screen
            continue;
        }
        poll_buttons();
        if(timer) update();
    }
}
