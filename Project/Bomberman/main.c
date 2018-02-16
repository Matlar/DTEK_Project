#include "header.h"

uint8_t pixels[512];
uint8_t position_state[32][8];
Bomb bombs[2][MAX_BOMBS];

Player p1, p2;
enum GameState state;

/* Taken from Lab3 */
void init_program(void) {
    /*
    This will set the peripheral bus clock to the same frequency
    as the sysclock. That means 80 MHz, when the microcontroller
    is running at 80 MHz. Changed 2017, as recommended by Axel.
    */
    SYSKEY = 0xAA996655;       /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA;       /* Unlock OSCCON, step 2 */
    while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000;      /* clear PBDIV bit <0,1> */
    while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
    SYSKEY = 0x0;              /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    /* SPI2STAT bit SPIROV = 0; */
    SPI2STATCLR = 0x40;
    /* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
    /* SPI2CON bit MSTEN = 1; */
    SPI2CONSET = 0x20;
    /* SPI2CON bit ON = 1; */
    SPI2CONSET = 0x8000;
}

/* Set up timer 2 with a 0.025 delay */
void init_timer(void) {
    PR2 = 7813;     // set period of timer 2, this is 0.025 sec with 256 prescaling
    TMR2 = 0;       // init timer to 0
    T2CON = 0x8070; // set prescaling of timer 2 to 256 and start timer
}

/* Get the state of the 4 buttons */
int getbtns(void) {
    int btn1 = (PORTF >> 1) & 1;
    // merge value of btn2-4 with btn1 into one int
    // with the values in the 4 lowest bits
    return ((PORTD >> 4) & 0xE) | btn1;
}

/* Sets a pixel at (x,y) to on or off depending on val */
void pixel(int x, int y, int val) {
    int page_offset = y / 8;
    int index = page_offset * 128 + x;  // find position in array
    int bit_pos = 0x1 << (y % 8);       // find position in byte

    if (val) pixels[index] |=  bit_pos; // set bit to 1
    else     pixels[index] &= ~bit_pos; // set bit to 0
}

/* Clears a 4x4 square at the x,y position (internal coordinates) */
void delete_pixels(int x, int y) {
    x *= 4; y *= 4; // convert from internal- to pixel-coord
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            pixel(x+i, y+j, 0);
        }
    }
}

/* Updates LED with the given value */
void update_led(uint8_t val) {
    PORTE &= ~0xFF; // clear lower 8
    PORTE |= val;   // set lower 8 to our value
}

/* Update function for the start screen state */
void start_screen(void) {
    int led1 = 1, led2 = 0x80;
    int timer_delay = 0, i;
    while(1) {
        /* Poll input */
        if (BUTTON(1)) {
            state = GAME;
            delay(3000000);
        }
        if (state != START) return;
        /* Poll input */

        if (!TIMER) continue;
        RESET_TIMER;
        if(timer_delay++ < TIMER_DELAY * 4) continue;
        timer_delay = 0;

        /* Update LED-pattern*/
        int step = led1 == 0x8 ? 2 : 1; // fixes 'stopping' in middle
        led1 <<= step;
        led2 >>= step;
        if (led1 == 0x80) {
            // reset animation
            led1 = 1;
            led2 = 0x80;
        }
        update_led(led1 | led2);

        /* Blinking animation for text "BTN2" */
        update_display(start);
        for (i = 0; i < 23; ++i)
            start[427 + i] ^= btn2[i];
        update_display(start);
    }
}

/* Update function for the end screen state */
void end_screen(void) {
    int led_show = 0, timer_delay = 0, i;
    while(1) {
        /* Poll input */
        if (BUTTON(1)) {
            state = GAME;
            delay(3000000);
        }
        if (state != END) return;
        /* Poll input */

        if (!TIMER) continue;
        RESET_TIMER;
        if (timer_delay++ < TIMER_DELAY * 4) continue;
        timer_delay = 0;

        /* Update LED-pattern */
        update_led(led_show ? 0xAA : ~0xAA);
        led_show = !led_show;

        /* Blinking animation for text "BTN2" */
        update_display(end);
        for (i = 0; i < 23; ++i)
            end[427 + i] ^= btn2[i];
        update_display(end);
    }
}

int main(void) {
    init_program();
    init_timer();
    init_display();

    state = START;
    update_display(start);
    while(1) {
        switch (state) {
            case START: start_screen(); break;
            case GAME:  run_game();     break;
            case END:   end_screen();   break;
        }
    }
    return 0;
}
