/*
 * potToDigConvert.c
 *
 *  Created on: Apr 24, 2023
 *      Author: millereb and Taylor DiSalvo
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "msp.h"
#include "potToDigConvert.h"

static volatile bool resultReady = false;
static volatile uint16_t curADCResult;

void initConversion(void)
{
    POT_PORT1->DIR &= ~(BIT2+POT1_BIT+POT2_BIT);
    POT_PORT1->SEL0 |= (BIT2+POT1_BIT+POT2_BIT);
    POT_PORT1->SEL1 |= (BIT2+POT1_BIT+POT2_BIT);


    /* Configure ADC (CTL0 and CTL1) registers for:
     *      clock source - default MODCLK, clock prescale 1:1,
     *      sample input signal (SHI) source - software controlled (ADC14SC),
     *      Pulse Sample mode with sampling period of 16 ADC14CLK cycles,
     *      Single-channel, single-conversion mode, 12-bit resolution,
     *      ADC14 conversion start address ADC14MEM1, and Low-power mode
     */
    ADC14->CTL0 = ADC14_CTL0_SHP        // Pulse Sample Mode
            | ADC14_CTL0_SHT0__16       // 16 cycle sample-and-hold time (for ADC14MEM1)
            | ADC14_CTL0_PDIV__1        // Predivide by 1
            | ADC14_CTL0_DIV__1         // /1 clock divider
            | ADC14_CTL0_SHS_0          // ADC14SC bit sample-and-hold source select
            | ADC14_CTL0_SSEL__MODCLK   // clock source select MODCLK
            | ADC14_CTL0_ON;            // ADC14 on

    ADC14->CTL0 |= ADC14_CTL0_MSC; /*!< ADC14 multiple sample and conversion */

    ADC14->CTL0 |= ADC14_CTL0_CONSEQ_1;         // Sequence of channels

    ADC14->CTL1 = ADC14_CTL1_RES__12BIT         // 12-bit conversion results
            | (0x1 << ADC14_CTL1_CSTARTADD_OFS) // ADC14MEM1 - conversion start address
            | ADC14_CTL1_PWRMD_2;               // Low-power mode


    // TODO Configure ADC14MCTL1 as storage register for result
    //          Single-ended mode with Vref+ = Vcc and Vref- = Vss,
    //          Input channel - A15, and comparator window disabled

    ADC14->MCTL[1] = 0x0001; //5.4
    ADC14->MCTL[2] = 0x0000; //5.5
    ADC14->MCTL[3] = 0X0003; //5.2

    ADC14->MCTL[3] |= 0b10000000;   //turn on End of Sequence bit

    // DONE Enable ADC conversion complete interrupt for ADC14MEM1
    ADC14->IER0 = 0b0010;

    //DONE Enable conversion
    ADC14->CTL0 |= ADC14_CTL0_ENC;

    // Enable ADC interrupt in NVIC module
    NVIC->ISER[0] = (1 << ADC14_IRQn);

    // Enable global interrupt
    __enable_irq();
}


void enableConversion(void) {
    ADC14->CTL0 |= ADC14_CTL0_ENC   // enable conversion
            | ADC14_CTL0_SC;        // start conversion
}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    // Check if interrupt triggered by ADC14MEM1 conversion value loaded
    //  Not necessary for this example since only one ADC channel used
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1) {
        curADCResult = ADC14->MEM[MEMORYCHANNEL];

        resultReady = true;
        // not necessary to clear flag because reading ADC14MEMx clears flag
    }
}
