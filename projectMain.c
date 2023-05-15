


#include "msp.h"
#include <stdio.h>  //printf(); sprintf();
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "csHFXT.h"
#include "csLFXT.h"

#include "lcd.h"
#include "speaker.h"
#include "servoDriver.h"
#include "potToDigConvert.h"

uint16_t mainNoteLengths[10];
uint16_t mainNotePeriods[10];

char lcdMessage[200];

void sendLCDMessage(char *message) {
    clearDisplay();
    setLineNumber(0x00);

    int i = 0;
    while (message[i] != 0) {
      printChar(message[i]);
      i++;
    }
}


void playMusic() {

}


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	/*
	 * INITIALIZE ALL HARDWARE
	 */

    //configure LCD
    configLCD(48000000);
    initLCD();

	//configure timers
	configHFXT();
	configLFXT();

	sprintf(lcdMessage, "Hi there hello");
	sendLCDMessage(lcdMessage);

	configureSpeaker();

	/*
	 * MAIN LOOP
	 */
	while(1) {
	    play();
	}

}
