


#include "msp.h"
#include <stdio.h>  //printf(); sprintf();
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "csHFXT.h"
#include "csLFXT.h"

#include "lcd.h"
#include "speaker.h"
#include "servoDriver.h"
#include "potToDigConvert.h"
#include "music.h"



uint16_t mainNoteLengths[MAXNOTES];
uint16_t mainNotePeriods[MAXNOTES];
uint16_t playNote;

uint16_t string1Note;
char note1;
char octave1;
uint16_t string2Note;
char note2;
char octave2;
uint16_t string3Note;
char note3;
char octave3;

char lcdMessage[200];

char lcdSetting; // 1 = showing string values, 2 = showing saved songs, 3 = welcome message
uint16_t lcdTime;

#define  Frequency10Hz 3200  // 50ms*32/ms = 1600

void sendLCDMessage(char *message) {
    clearDisplay();
    setLineNumber(0x00);

    int i = 0;
    while (message[i] != 0) {
        if (message[i] == '/') {
            setLineNumber(0x40);
        } else {
            printChar(message[i]);
        }

        i++;
    }
}

void configureSwitches(void) {
    //configure control switches
    P1->SEL0 &= BIT5+BIT6+BIT7;
    P1->SEL1 &= BIT5+BIT6+BIT7;
    P1->OUT |= BIT5+BIT6+BIT7;
    P1->REN |= BIT5+BIT6+BIT7;

    //configure string switches, select button
    P3->SEL0 &= BIT5+BIT6+BIT7+BIT0;
    P3->SEL1 &= BIT5+BIT6+BIT7+BIT0;
    P3->OUT |= BIT5+BIT6+BIT7+BIT0;
    P3->REN |= BIT5+BIT6+BIT7+BIT0;


}

bool playSwitchPressed(void) {
    char switchStatus = (P1->IN >> 5) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    } else {
        //button is pressed
        return true;
    }
}

bool stopSwitchPressed(void)
{
    char switchStatus = (P1->IN >> 6) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    }
    else {
        //button is pressed
        return true;
    }
}

bool saveSwitchPressed(void)
{
    char switchStatus = (P1->IN >> 7) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    }
    else {
        //button is pressed
        return true;
    }
}

bool selectSwitchPressed(void)
{
    char switchStatus = (P3->IN >> 0) & 1;
    if (switchStatus == 1) {
        //button is not pressed
        return false;
    }
    else {
        //button is pressed
        return true;
    }
}

bool stringOnePressed(void) {
    char switchStatus = (P3->IN >> 5) & 1;
    if (switchStatus == 1) {
       //button is not pressed
       return false;
    }
    else {
       //button is pressed
       return true;
    }
}
bool stringTwoPressed(void) {
    char switchStatus = (P3->IN >> 6) & 1;
    if (switchStatus == 1) {
       //button is not pressed
       return false;
    }
    else {
       //button is pressed
       return true;
    }
}
bool stringThreePressed(void) {
    char switchStatus = (P3->IN >> 7) & 1;
    if (switchStatus == 1) {
       //button is not pressed
       return false;
    }
    else {
       //button is pressed
       return true;
    }
}

uint16_t setStringOneNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        note1 = 'G';
        return NOTEG3; //first tone
    } else if (adcValue < 2000) {
        note1 = 'A';
        return NOTEA3; //second tone
    } else if (adcValue < 3000) {
        note1 = 'B';
        return NOTEB3; //third tone
    } else if (adcValue < 4000) {
        note1 = 'C';
        return NOTEC4; //fourth tone
    } else {
        note1 = 'D';
        return NOTED4; //fifth tone
    }
}

uint16_t setStringTwoNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        note2 = 'E';
        return NOTEE4; //first tone
    } else if (adcValue < 2000) {
        note2 = 'F';
        return NOTEF4; //second tone
    } else if (adcValue < 3000) {
        note2 = 'G';
        return NOTEG4; //third tone
    } else if (adcValue < 4000) {
        note2 = 'A';
        return NOTEA5; //fourth tone
    } else {
        note2 = 'B';
        return NOTEB5; //fifth tone
    }
}

uint16_t setStringThreeNote(uint16_t adcValue) {
    if(adcValue < 1000) {
        note3 = 'C';
        return NOTEC5; //first tone
    } else if (adcValue < 2000) {
        note3 = 'D';
        return NOTED5; //second tone
    } else if (adcValue < 3000) {
        note3 = 'E';
        return NOTEE5; //third tone
    } else if (adcValue < 4000) {
        note3 = 'F';
        return NOTEF5; //fourth tone
    } else {
        note3 = 'G';
        return NOTEG5; //fifth tone
    }
}


uint16_t determineNoteLength(void) {

}

void ConfigureTimerA0CCROInterrupt(void) {

    /* Configure Timer_A1 and CCRs */
     // Set initial period in CCR0 register. This assumes timer starts at 0
     TIMER_A2->CCR[0] = Frequency10Hz;
     // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
     TIMER_A2->CCTL[0]=0x0010;   //0b0000 0000 0001 CCIE=1 0000
     // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
     TIMER_A2->CTL=0x0112;  //0b0000 0001 ACLK 0001 UP 0100

     /* Configure global interrupts and NVIC */
     NVIC->ISER[0] |= (1)<<TA2_0_IRQn;

     __enable_irq();
}

void setTestNotes(void) {
    mainNoteLengths[0] = 35000;
    mainNoteLengths[1] = 35000;
    mainNoteLengths[2] = 35000;
    mainNoteLengths[3] = 35000;
    mainNoteLengths[4] = 35000;
    mainNoteLengths[5] = 35000;
    mainNoteLengths[6] = 35000;
    mainNoteLengths[7] = 35000;
    mainNoteLengths[8] = 35000;
    mainNoteLengths[9] = 35000;

}

void mainLCD() {
    //display welcome message
    if(lcdSetting == '3') {
        //increase the timer value
        lcdTime++;

        if(lcdTime > 50) {
            //change mode to regular string display
            lcdSetting = '1';
        } else {
            sprintf(lcdMessage, "Welcome!/Play Some Music!");
            sendLCDMessage(lcdMessage);
        }
    }

    //standard display showing note values
    if(lcdSetting == '1') {
        sprintf(lcdMessage, "1  2  3/%c  %c  %c", note1, note2, note3);
        sendLCDMessage(lcdMessage);
    }
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


	lcdTime = 0;
	lcdSetting = '3'; // start LCD in welcome mode

	configureSpeaker();
	playNote = 0;
	//setTestNotes();
	note1 = 'A';
	note2 = 'B';
	note3 = 'C';
	string1Note = 0;
	string2Note = 1;
	string3Note = 2;

	ConfigureTimerA0CCROInterrupt();

	initConversion();

	configureSwitches();

	/*
	 * MAIN LOOP
	 */
	while(1) {

	}

}

void TA2_0_IRQHandler(void)
{
    if (TIMER_A2->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //clear interrupt flag
        enableConversion();

        //get current values of the string potentiometers
        while((ADC14->IFGR0 & ADC14_IFGR0_IFG1)==0) {};
        uint16_t string1 = ADC14->MEM[1];
        uint16_t string2 = ADC14->MEM[2]; //get string values
        uint16_t string3 = ADC14->MEM[3];

        //determine if any control buttons are pressed
        if(playSwitchPressed()) {
            setMusic(mainNotePeriods, mainNoteLengths, playNote);
            play();
        }
        if(stopSwitchPressed()) {
            stop();
        }
        if(saveSwitchPressed()) {
            //wait for switch to be released
            bool selectButton = false;
            while(saveSwitchPressed()) {
                if(selectSwitchPressed()) {
                    selectButton = true;
                    break;
                }
            }

            //if the section button was pressed, clear song
            if(selectButton) {
                int i=0;
                for(i=0; i<MAXNOTES; i++) {
                    mainNotePeriods[i] = 0;
                    mainNoteLengths[i] = 0;
                    playNote = 0;
                }
            } else {
                //otherwise ask to save the song or enter saves
            }
        }

        //run main function to update LCD
        mainLCD();

        string1Note = setStringOneNote(string1);
        string2Note = setStringTwoNote(string2);
        string3Note = setStringThreeNote(string3);

        uint16_t holdLength = 0;
        uint16_t holdTimer = 0;
        //determine if the string buttons are pressed
        if (stringOnePressed())
        {
            //wait fur button release
            while(stringOnePressed()) {
                holdTimer++;
                if(holdTimer%4==0) holdLength++;
            }
            mainNoteLengths[playNote] = holdLength;
            mainNotePeriods[playNote] = string1Note;
            playNote++;
        }
        if (stringTwoPressed())
        {
            //wait fur button release
            while (stringOnePressed()) {
                holdTimer++;
                if (holdTimer % 4 == 0)
                    holdLength++;
            }
            mainNoteLengths[playNote] = holdLength;
            mainNotePeriods[playNote] = string1Note;
            playNote++;
        }
        if (stringThreePressed())
        {
            //wait fur button release
            while (stringOnePressed())
            {
                holdTimer++;
                if (holdTimer % 4 == 0)
                    holdLength++;
            }
            mainNoteLengths[playNote] = holdLength;
            mainNotePeriods[playNote] = string1Note;
            playNote++;
        }

        printf("\n\r value1: %d", string1Note);
        printf(" value2: %d", string2Note);
        printf(" value3: %d", string3Note);
    }
}
