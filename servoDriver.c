/*! \file */
/*!
 * servoDriverTemplate.c
 * ECE230-02 Spring 2022-2023
 * Date: April 11, 2023
 * Description: Servo motor driver for MSP432P4111 Launchpad.
 *              Assumes SMCLK configured with 48MHz HFXT as source.
 *              Uses Timer_A2 and P5.6 (TA2.1)
 *
 **                 MSP432P4111
 *             -------------------
 *         /|\|                   |
 *          | |                   |
 *          --|RST                |
 *            |      (TA2.1) P5.6 |----> Servo
 *            |                   |
 *            |              PJ.2 |------
 *       S1-->|P1.1               |     |
 *            |                   |    HFXT @ 48MHz
 *            |                   |     |
 *            |              PJ.3 |------
 */

#include "servoDriver.h"
#include "msp.h"

/* Global Variables  */
uint16_t pulseWidthTicks = SERVO_MIN_ANGLE;


void initServoMotor(void) {
    // DONE configure servo pin (P5.6) for primary module function (TA2.1),
    //  output, initially LOW
    ServoControl_Port -> DIR |= ServoControl_Pin;
    ServoControl_Port -> SEL0 |= ServoControl_Pin;
    ServoControl_Port -> SEL1 &= ~ServoControl_Pin;
    ServoControl_Port -> OUT &= ~ServoControl_Pin;

    /* Configure Timer_A2 and CCR1 */
    // Set period of Timer_A2 in CCR0 register for Up Mode
    TIMER_A2->CCR[0] = SERVO_TMR_PERIOD;
    // Set initial positive pulse-width of PWM in CCR1 register
    TIMER_A2->CCR[1] = SERVO_MIN_ANGLE;

    // DONE configure TA2CCR1 for Compare mode, Reset/Set output mode on P5.6, with interrupt disabled
    // assign Timer control register value in binary 0b10101001001
    TIMER_A2->CCTL[1] = 0b11100000;

    // Configure Timer_A2 in Up Mode, with source SMCLK, prescale 16:1, and
    //  interrupt disabled  -  tick rate will be 3MHz (for SMCLK = 48MHz)
    // DONE configure Timer_A2 (requires setting control AND expansion register)
    // Assign control register value in binary 0b10101001001
    TIMER_A2->CTL = 0b1011010000;
    TIMER_A2->EX0 = 0x002;

}

void incrementTenDegree(void) {
    // update pulse-width for <current angle> + <10 degrees>
    pulseWidthTicks += TEN_DEGREE_TICKS;
    if (pulseWidthTicks > SERVO_MAX_ANGLE) {
        pulseWidthTicks = SERVO_MIN_ANGLE;
    }
    // DONE update CCR1 register to set new positive pulse-width
    TIMER_A2->CCR[1] = pulseWidthTicks;
}

//move the servo from min position to some ratio of the max position
void setServoRatio(double ratio) {
    TIMER_A2->CCR[1] = pulseWidthTicks + ((SERVO_MAX_ANGLE-SERVO_MIN_ANGLE)*ratio*1.2);
}

void resetServo() {
    TIMER_A2->CCR[1] = SERVO_MIN_ANGLE;
}

void setServoAngle(int8_t angle) {
    // NOT NEEDED FOR EXERCISE - but would be useful function for driver
}
