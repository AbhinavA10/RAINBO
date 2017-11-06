/*==============================================================================
    Project: RAINBO-Starter
    Version: 1.0				Date: October 18, 2017
    Target: RAINBO				Processor: PIC12F1840

 RAINBO NeoPixel driver circuit with IR control.
 =============================================================================*/

#include    "xc.h"              // XC compiler general include file

#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include	"RAINBO.h"			// Include user-created constants and functions

// Byte variables for neopixel functions

unsigned char red = 0; // RGB colour bytes and default starting colour
unsigned char green = 0;
unsigned char blue = 0;

unsigned char type = "IR";

const char maxLEDs = 60; // Number of LEDs in the string

unsigned char temp; // Temporary byte storage
unsigned char i; // Generic index counter

unsigned char adConvert(unsigned char chan) {
    ADON = 1; // Turn A-D converter on
    ADCON0 = (ADCON0 & 0b10000011); // Change the conversion channel by wiping
    ADCON0 = (ADCON0 | chan); // CHS bits and ORing with selected channel
    __delay_us(2); // Give input time to settle
    GO = 1; // Start the conversion

    while (GO); // Wait for the conversion to finish (GO=0)

    ADON = 0; // Turn A-D converter off and
    return (ADRESH); // return the upper 8-bits (MSB) of the result
}

/*==============================================================================
 *	neoRGB - sets requested number of Neopixel LEDs to same RGB value.
 =============================================================================*/

void neoRGB(unsigned char red, unsigned char green, unsigned char blue, unsigned char leds) {
    for (leds; leds != 0; leds--) { // Repeat all 24 colour bits for each LED
        temp = green; // Copy green byte, prepare to shift MSB first
        for (i = 8; i != 0; i--) { // PWM each bit in assembly code for speed
            asm("bsf LATA,4"); // Make N2 output high
            asm("nop"); // Wait one instruction cycle
            asm("btfss _temp,7"); // If MSB == 1, skip next instruction
            asm("bcf LATA,4"); // Make N2 output low (0)
            asm("nop"); // Pad by one instruction cycle for timing
            asm("lslf _temp,f"); // Shift next bit into MSB position
            asm("bcf LATA,4"); // Make N2 output low (1)
        }
        temp = red; // Red next, same as green.
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
        temp = blue; // Blue last. 
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
    }
}

void changeLEDS() {
    if (red == 254 && blue == 0) type = 'IG';
    if (green == 254) type = 'DR';
    if (green == 254 && red == 0) type = 'IB';
    if (blue == 254) type = 'DG';
    if (blue == 254 && green == 0) type = 'IR';
    if (red == 254) type = 'IG';
    switch (type) {
        case "IR":
            red++;
            break;
        case "IG":
            green++;
            break;
        case "IB":
            blue++;
            break;
        case "DR":
            red--;
            break;
        case "DG":
            green--;
            break;
        case "DB":
            blue--;
            break;
    }
}

int main(void) // Start of program
{
    init(); // Initialize oscillator and I/O Ports
    __delay_us(200);


    while (1) {
        neoRGB(red, green, blue, maxLEDs);
        changeLEDS();

    }
}

// change all colours gradually