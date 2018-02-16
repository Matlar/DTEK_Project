#include <pic32mx.h>
#include <stdint.h>
#include "header.h"

#define COMMAND_MODE (PORTFCLR = 0x10)
#define DATA_MODE    (PORTFSET = 0x10)
#define RESET_ON     (PORTGCLR = 0x200)
#define RESET_OFF    (PORTGSET = 0x200)
#define VDD_ON       (PORTFCLR = 0x40)
#define VBAT_ON      (PORTFCLR = 0x20)

///////////////////////////
// This file includes functions related to the display
///////////////////////////

void delay(int num) {
    int i;
    for(i = num; i > 0; i--);
}

/* Sends a byte of data to the display */
void send_byte(uint8_t byte) {
    while(SPI2STAT & 0x8 == 0);
    SPI2BUF = byte;
    while(SPI2STAT & 0x1 == 0);
}

/* Taken from "hello-display"-example */
void init_display(void) {
    COMMAND_MODE;
    delay(10);
    VDD_ON; // power to logic of display
    delay(1000000);

    send_byte(0xAE); // display off
    RESET_ON;
    delay(10);
    RESET_OFF;
    delay(10);

    /* We added this */
    send_byte(0x20); // set memory mode
    send_byte(0x0);  // to Horizontal addressing mode
    /* We added this */

    send_byte(0x8D); // CHARGEPUMP
    send_byte(0x14); // value to CHARGEPUMP?

    send_byte(0xD9); // SETPRECHARGE
    send_byte(0xF1); // value to SETPRECHARGE

    VBAT_ON; // power to actual display
    delay(10000000);

    send_byte(0xA1); // set segment remap
    send_byte(0xC8); // set COM output scan direction

    send_byte(0xDA); // SETCOMPINS
    send_byte(0x20); // value to SETCOMPINS

    send_byte(0xAF); // display ON

    /* We added this */
    delay(10);
    DATA_MODE; // we can stay in data mode for the rest of the program
}

/* Sends the content of the array to the display */
void update_display(uint8_t *pix) {
    int i;
    for (i = 0; i < 512; ++i)
        send_byte(pix[i]);
}
