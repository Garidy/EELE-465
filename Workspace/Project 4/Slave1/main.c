#include <msp430.h> 


int Data;
char count;
char pattern1 = 127;
char pattern2 = 24;
void display(char pattern);

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
    UCB0CTLW0 |= UCSWRST; //RESET


	UCB0CTLW0 |= UCMODE_3; //SELECT I2C
    UCB0I2COA0 = 0x01;     //SLAVE ADDRESS
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

    UCB0CTLW0 &= ~UCSWRST;              // OUT OF RESET

    UCB0IE |= UCRXIE0;                  //ENABLE I2C RX IRQ

    __enable_interrupt();


    while(1){
        int i;
        int temp;


        if(Data == 'A'){            //Pattern A
            P1OUT |= BIT0;  //Set LED 1
            P1OUT &= ~BIT1; //clear LED 2
            P1OUT |= BIT4;  //Set LED 3
            P1OUT &= ~BIT5; //clear LED 4
            P1OUT |= BIT6;  //Set LED 5
            P1OUT &= ~BIT7; //clear LED 6
            P2OUT |= BIT6;  //Set LED 7
            P2OUT &= ~BIT7; //clear LED 8

        }
        else if (Data == 'B'){     //Pattern B
            count++;
            display(count);

            for(i=0; i<10000; i=i+1){}  //not 1s

        }
        else if (Data == 'C'){    //Pattern C
            pattern1 = (pattern1 << 1) | (pattern1 >> (8 - 1));
            display(pattern1);
            for(i=0; i<10000; i=i+1){}  //not 1s


        }
        else if (Data == 'D'){    //Pattern D
            temp++;
            if(((temp % 7) == 0) || ((temp % 7) == 6)){
                pattern2 = 24;
            }else if(((temp % 7) == 2) || ((temp % 7) == 5)){
                pattern2 = 36;
            }else if(((temp % 7) == 2) || ((temp % 7) == 4)){
                pattern2 = 66;
            }else if(((temp % 7) == 3)){
                pattern2 = 129;
            }
            display(pattern2);
            for(i=0; i<10000; i=i+1){}  //not 1s

        }

        /*
        P1OUT |= BIT0;  //Set LED 1
        P1OUT |= BIT1;  //Set LED 2
        P1OUT |= BIT4;  //Set LED 3
        P1OUT |= BIT5;  //Set LED 4
        P1OUT |= BIT6;  //Set LED 5
        P1OUT |= BIT7;  //Set LED 6
        P2OUT |= BIT6;  //Set LED 7
        P2OUT |= BIT7;  //Set LED 8
    */

    }
	return 0;
}

void display(char pattern){
    if(pattern & BIT0){
        P1OUT |= BIT0;  //Set LED 1
    }else{
        P1OUT &= ~BIT0;  //clear LED 1
    }

    if(pattern & BIT1){
        P1OUT |= BIT1;  //Set LED 2
    }else{
        P1OUT &= ~BIT1; //clear LED 2
    }

    if(pattern & BIT2){
        P1OUT |= BIT4;  //Set LED 3
    }else{
        P1OUT &= ~BIT4; //clear LED 3
    }

    if(pattern & BIT3){
        P1OUT |= BIT5;  //Set LED 4
    }else{
        P1OUT &= ~BIT5; //clear LED 4
    }

    if(pattern & BIT4){
        P1OUT |= BIT6;  //Set LED 5
    }else{
        P1OUT &= ~BIT6; //clear LED 5
    }

    if(pattern & BIT5){
        P1OUT |= BIT7;  //Set LED 6
    }else{
        P1OUT &= ~BIT7; //clear LED 6
    }

    if(pattern & BIT6){
        P2OUT |= BIT6;  //Set LED 7
    }else{
        P2OUT &= ~BIT6; //clear LED 7
    }

    if(pattern & BIT7){
        P2OUT |= BIT7;  //Set LED 8
    }else{
        P2OUT &= ~BIT7; //clear LED 8
    }
}


//--------------------------------------------
//--ISRs
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    UCB0IE &= ~UCRXIE0;
    if(UCB0RXBUF != '\0') {
        Data = UCB0RXBUF;
    }
    UCB0IE |= UCRXIE0;
}
