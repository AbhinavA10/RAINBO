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

unsigned char cRed = 254; // RGB colour bytes and default starting colour
unsigned char cGreen = 254;
unsigned char cBlue = 0;

const char maxLEDs = 30; // Number of LEDs in the string
unsigned char type = 2;

unsigned char index = 0;

unsigned char arcColours[3][6] = {//ROYGBV
    {255, 255, 255, 0, 0, 238},
    {0, 140, 215, 255, 0, 130},
    {0, 0, 0, 0, 255, 238}
};
// red is first, then green, then blue

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
    for (leds; leds != 0; leds--) // Repeat all 24 colour bits for each LED
    {
        temp = green; // Copy green byte, prepare to shift MSB first
        for (i = 8; i != 0; i--) // PWM each bit in assembly code for speed
        {
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

void neoArray(unsigned char leds) {
    index = 0;
    for (leds; leds != 0; leds--) // Repeat all 24 colour bits for each LED
    {
        index++;
        if (index == 6) {
            index = 0;
        }
        temp = arcColours[1][index];
         // Copy green byte, prepare to shift MSB first
        for (i = 8; i != 0; i--) // PWM each bit in assembly code for speed
        {
            asm("bsf LATA,4"); // Make N2 output high
            asm("nop"); // Wait one instruction cycle
            asm("btfss _temp,7"); // If MSB == 1, skip next instruction
            asm("bcf LATA,4"); // Make N2 output low (0)
            asm("nop"); // Pad by one instruction cycle for timing
            asm("lslf _temp,f"); // Shift next bit into MSB position
            asm("bcf LATA,4"); // Make N2 output low (1)
        }
        temp = arcColours[0][index]; // Red next, same as green.
        for (i = 8; i != 0; i--) {
            asm("bsf LATA,4");
            asm("nop");
            asm("btfss _temp,7");
            asm("bcf LATA,4");
            asm("nop");
            asm("lslf _temp,f");
            asm("bcf LATA,4");
        }
        temp = arcColours[2][index]; // Blue last. 
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
    if (cRed == 254 && cGreen == 0 && cBlue == 0) type = 2; // IG
    if (cRed == 254 && cGreen == 254 && cBlue == 0) type = 4; //DR
    if (cRed == 0 && cBlue == 0 && cGreen == 254) type = 3; //IB
    if (cBlue == 254 && cGreen == 254 && cRed == 0) type = 5; //DG
    if (cBlue == 254 && cGreen == 0 && cRed == 0) type = 1; //IR
    if (cBlue == 254 && cRed == 254 && cGreen == 0) type = 6; //DB
    switch (type) {
        case 1:
            cRed++;
            break;
        case 2:
            cGreen++;
            break;
        case 3:
            cBlue++;
            break;
        case 4:
            cRed--;
            break;
        case 5:
            cGreen--;
            break;
        case 6:
            cBlue--;
            break;
    }
}

int main(void) // Start of program
{
    init(); // Initialize oscillator and I/O Ports
    __delay_us(200);


    while (1) {
        // neoRGB(cRed, cGreen, cBlue, maxLEDs);
        neoArray(maxLEDs);
        //SLEEP();
        //changeLEDS();
        __delay_us(50);
    }
}