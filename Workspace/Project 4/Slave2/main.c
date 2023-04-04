/*     James Wurst
 *     EELE 465
 *     Slave 2 Code
 *     2 March 2023
 */

#include <msp430.h>
#include "lcd.h"

//int j;
//int Delay;
int Data;
int prevInput;
int KeyPressFlag;
int DataPos = 0;



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    UCB0CTLW0 |= UCSWRST; //RESET


    UCB0CTLW0 |= UCMODE_3; //SELECT I2C
    UCB0I2COA0 = 0x02;     //SLAVE ADDRESS
    UCB0CTLW0 &= ~UCMST;   //slave MODE
    UCB0CTLW0 &= ~UCTR;      //RECEIVE MODE
    //UCB0I2COA0 |= UCGCEN;   // general call response enable
    UCB0I2COA0 |= UCOAEN;   // slave address enable

    UCB0CTLW0 &= ~UCTR;      //RECEIVE MODE
    UCB0CTLW1 &= ~UCSWACK;  // auto acknowledge
    UCB0TBCNT = 0x01;      // Length of Receiving data

    //UCB0CTLW1 &= ~UCTXIFG0;
    P1SEL1 &= ~BIT3;    //P1.3 = SCL
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;     //P1.2 = SDA
    P1SEL0 |= BIT2;

    //---------------------- ADC Setup -------------------------------
    //-- Configure Ports
    P1DIR |= BIT4;      // set P1.4 as output
    P1DIR |= BIT5;      // set P1.5 as output
    P1DIR |= BIT6;      // set P1.6 as output
    P1DIR |= BIT7;      // set P1.7 as output

    P2DIR |= BIT6;      // set P2.6 as output
    P2DIR |= BIT7;      // set P2.7 as output

    PM5CTL0 &= ~LOCKLPM5;   // turn on GPIO

    UCB0CTLW0 &= ~UCSWRST;              // OUT OF RESET

    UCB0IE |= UCRXIE0;                  //ENABLE I2C RX IRQ

    __enable_interrupt();

    LcdInit();  //initialize LCD
    LcdTempInit();

    //-- Main Loop
    while(1)
    {


}


    return 0;
}

//-- Interrupt Service Routines -------------------------------
//--------------------------------------------
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    UCB0IE &= ~UCRXIE0;
    if(UCB0RXBUF != '\0') {
        //prevInput = Data;
        //Data = UCB0RXBUF;

        Data = UCB0RXBUF;
        DataPos++;

    }
    if (DataPos == 1){
        //set pos to 08
        BitSet(0,0x08);
        BitSet(0,0x08);
        KeyEntered(Data);
        //set pos to 44
        BitSet(0,0x0C);
        BitSet(0,0x04);
    }
    if (DataPos > 1 && DataPos < 5){
        KeyEntered(Data);
    }
    if (DataPos == 4){
        //set pos to 4A
        BitSet(0,0x0C);
        BitSet(0,0x0A);
    }
    if (DataPos > 5){
        //set pos to 4A
        KeyEntered(Data);
    }
    if (DataPos == 6){
        //decimal
       BitSet(1,0x02);
       BitSet(1,0x0E);
    }
    if (DataPos == 7){
        DataPos = 0;
    }
    UCB0IE |= UCRXIE0;
}



