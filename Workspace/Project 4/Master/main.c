#include <msp430.h> 

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	//setup B0 for I2C
	UCB0CTLW0 |= UCSWRST;   // put in SW RST
	UCB0CTLW0 |= UCSSEL_3;  // choose SMCLK
	UCB0BRW = 10;           // set prescalar to 10

	UCB0CTLW0 |= UCMODE_3;  // put into I2C mode
	UCB0CTLW0 |= UCMST;     // set as master
	UCB0CTLW0 |= UCTR;      // default Tx mode
	UCB0I2CSA = 0x68;       // set slave address

	UCB0CTLW1 |= UCASTP_2;   // auto STOP mode
	UCB0TBCNT |= 1;         // count = 1 byte

    //-- setup ports
	P1SEL1 &= ~BIT3;   //P1.3 SCL (48)
	P1SEL1 |= BIT3;

    P1SEL1 &= ~BIT2;   //P1.2 SDA (1)
    P1SEL1 |= BIT2;

    //test leds

    P1DIR |= BIT0;  //config LED 1 to output
    P6DIR |= BIT6;  //config LED 2 to output
    P1OUT &= ~BIT0; //clear LED 1
    P6OUT &= ~BIT6; //clear LED 2

    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;   // take B0 out of SW RST


	//enable B0 TX0 IRQ
	UCB0IE |= UCTXIE0;  //local enable for TX0
	__enable_interrupt();   //enable maskables

	int i;

    while(1){
        P1OUT |= BIT0;
        P6OUT &= ~BIT6;

        UCB0CTLW0 |= UCTXSTT;   //manually start

        for(i=0; i<100; i=i+1){}//delay

    }
    return 0;

}


//--------------------------------------------
//--ISRs
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    UCB0TXBUF = 0xBB;

}


