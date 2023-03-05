#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    P1DIR |= BIT0;  //config LED 1 to output
    P1DIR |= BIT1;  //config LED 2 to output
    P1DIR |= BIT4;  //config LED 3 to output
    P1DIR |= BIT5;  //config LED 4 to output
    P1DIR |= BIT6;  //config LED 5 to output
    P1DIR |= BIT7;  //config LED 6 to output
    P2DIR |= BIT6;  //config LED 7 to output
    P2DIR |= BIT7;  //config LED 8 to output



    P1OUT &= ~BIT0; //clear LED 1
    P1OUT &= ~BIT1; //clear LED 2
    P1OUT &= ~BIT4; //clear LED 3
    P1OUT &= ~BIT5; //clear LED 4
    P1OUT &= ~BIT6; //clear LED 5
    P1OUT &= ~BIT7; //clear LED 6
    P2OUT &= ~BIT6; //clear LED 7
    P2OUT &= ~BIT7; //clear LED 8

    PM5CTL0 &= ~LOCKLPM5;


    while(1){
        P1OUT |= BIT0;  //Set LED 1
        P1OUT |= BIT1;  //Set LED 2
        P1OUT |= BIT4;  //Set LED 3
        P1OUT |= BIT5;  //Set LED 4
        P1OUT |= BIT6;  //Set LED 5
        P1OUT |= BIT7;  //Set LED 6
        P2OUT |= BIT6;  //Set LED 7
        P2OUT |= BIT7;  //Set LED 8

    }
	return 0;
}
