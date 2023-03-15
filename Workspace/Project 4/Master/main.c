#include <msp430.h> 

char keypad;
char Data_In =0;
char Packet[] = {0x03};

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //setup B0 for I2C
    UCB0CTLW0 |= UCSWRST;   // put in SW RST
    UCB0CTLW0 |= UCSSEL_3;  // choose SMCLK
    UCB0BRW = 10;           // set prescalar to 10

    UCB0CTLW0 |= UCMODE_3;  // put into I2C mode
    UCB0CTLW0 |= UCMST;     // set as master
    UCB0CTLW0 |= UCTR;      // default Tx mode
    UCB0I2CSA = 0x01;       // set slave address

    UCB0CTLW1 |= UCASTP_2;   // auto STOP mode
    UCB0TBCNT |= 1;         // count = 1 byte

    //-- setup ports
    P1SEL1 &= ~BIT3;   //P1.3 SCL (48)
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;   //P1.2 SDA (1)
    P1SEL0 |= BIT2;


    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;   // take B0 out of SW RST


    //enable B0 TX0 IRQ
    UCB0IE |= UCTXIE0;  //local enable for TX0
    //UCB0IE |= UCRXIE0;  // Rx
    __enable_interrupt();   //enable maskables

    int i;

    while(1){
        P1OUT |= BIT0;
        P6OUT &= ~BIT6;

        keypad = checkKeypad();

        //UCB0CTLW0 |= UCTR;      //Tx Mode
        UCB0CTLW0 |= UCTXSTT;   //manually start
        for(i=0; i<100; i=i+1){}

        //while(( UCB0IFG & UCSTPIFG ) == 0){}
        //    UCB0IFG &= ~UCSTPIFG;   //clear stop flag

        //UCB0CTLW0 &= ~UCTR;      //Rx Mode
       // UCB0CTLW0 |= UCTXSTT;   //manually start

        //while(( UCB0IFG & UCSTPIFG ) == 0){}
        //    UCB0IFG &= ~UCSTPIFG;   //clear stop flag

    }
    return 0;

}

char checkKeypad() {
    char buttonPressed = 0x00;      //init 0

    //D1 = P4.1
    P4REN |= BIT1;             //Enable Resistors
    P4OUT &= ~BIT1;            //Pull Up
    P4DIR &= ~BIT1;            //Input

    //D2 = P4.0
    P4REN |= BIT0;
    P4OUT &= ~BIT0;
    P4DIR &= ~BIT0;

    //D3 = P2.3
    P2REN |= BIT3;
    P2OUT &= ~BIT3;
    P2DIR &= ~BIT3;

    //D4 = P2.2
    P2REN |= BIT2;
    P2OUT &= ~BIT2;
    P2DIR &= ~BIT2;


    //D5 = P2.1
    P2DIR |= BIT1;              //Output
    P2OUT |= BIT1;              //High

    //D6 = P2.0
    P2DIR |= BIT0;
    P2OUT |= BIT0;

    //D7 = P1.7
    P1DIR |= BIT7;
    P1OUT |= BIT7;

    //D8 = P1.6
    P1DIR |= BIT6;
    P1OUT |= BIT6;


    if(P4IN & BIT1){
        buttonPressed = 0x08;
    }else if(P4IN & BIT0){
        buttonPressed = 0x04;
    }else if(P2IN & BIT3){
        buttonPressed = 0x02;
    }else if(P2IN & BIT2){
        buttonPressed = 0x01;
    }else{
        return '\0';
    };


    //D1 = P4.1
    P4DIR |= BIT1;              //Output
    P4OUT |= BIT1;              //High

    //D2 = P4.0
    P4DIR |= BIT0;
    P4OUT |= BIT0;

    //D3 = P2.3
    P2DIR |= BIT3;
    P2OUT |= BIT3;

    //D4 = P2.2
    P2DIR |= BIT2;
    P2OUT |= BIT2;


    //D5 = P2.1
    P2REN |= BIT1;              //Enable Resistors
    P2OUT &= ~BIT1;             //Pull Up
    P2DIR &= ~BIT1;             //Input


    //D6 = P2.0
    P2REN |= BIT0;              //Enable Resistors
    P2OUT &= ~BIT0;             //Pull Up
    P2DIR &= ~BIT0;             //Input

    //D7 = P1.7
    P1REN |= BIT7;              //Enable Resistors
    P1OUT &= ~BIT7;             //Pull Up
    P1DIR &= ~BIT7;             //Input

    //D8 = P1.6
    P1REN |= BIT6;              //Enable Resistors
    P1OUT &= ~BIT6;             //Pull Up
    P1DIR &= ~BIT6;             //Input


    if(P2IN & BIT1){
        buttonPressed = buttonPressed + 0x80;
    }else if(P2IN & BIT0){
        buttonPressed = buttonPressed + 0x40;
    }else if(P1IN & BIT7){
        buttonPressed = buttonPressed + 0x20;
    }else if(P1IN & BIT6){
        buttonPressed = buttonPressed + 0x10;
    }else{
        return '\0';
    };

    return buttonPressed;

}


//--------------------------------------------
//--ISRs
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{

    UCB0TXBUF = 0x0A;


}

