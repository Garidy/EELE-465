/*     James Wurst
 *     EELE 465
 *     Slave 2 Code
 *     2 March 2023
 */

#include <msp430.h> 
#include "lcd.h"

//int j;
//int Delay;



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //---------------------- ADC Setup -------------------------------
    //-- Configure Ports
    P1DIR |= BIT4;      // set P1.4 as output
    P1DIR |= BIT5;      // set P1.5 as output
    P1DIR |= BIT6;      // set P1.6 as output
    P1DIR |= BIT7;      // set P1.7 as output

    P2DIR |= BIT6;      // set P2.6 as output
    P2DIR |= BIT7;      // set P2.7 as output

    PM5CTL0 &= ~LOCKLPM5;   // turn on GPIO


    //-------------------------------------------------------
/*
    // Setup eUSCI_B0-------------------------------------------------------
    UCB0CTLW0 |= UCSWRST;
    UCB0CTLW0 |= UCSSEL_3;
    UCB0BRW = 10;
    UCB0CTLW0 |= UCMODE_3;
    UCB0CTLW0 |= UCMST;
    UCB0CTLW0 |= UCTR;
    UCB0I2CSA = 0x0068;
    UCB0CTLW1 |= UCASTP_2;
    UCB0TBCNT = sizeof(Packet);
*/
/*
    //Take eUSCI_A1 out of SW reset
    UCA1CTLW0 &= ~UCSWRST;

    // Enable Interrupts----------------------------------------------------
    UCB0IE |= UCRXIE0;
    UCB0IE |= UCTXIE0;
    UCA1IE |= UCRXIE;
*/
//    __enable_interrupt();   // enable maskable IRQs

    LcdInit();  //initialize LCD

    //-- Main Loop
    while(1)
    {

        KeyEntered('A');
        Delay(10000);
        KeyEntered('B');
        Delay(10000);

        while(1){
            //loop
        }


}


    return 0;
}

//-- Interrupt Service Routines -------------------------------



