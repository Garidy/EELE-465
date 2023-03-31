#include <msp430.h> 
#include <math.h>

char keypad;
char prevInput;

float tempC;
int tempK;

unsigned int globalIndex;                   // global index to send in I2C ISR
int readIndex = 0;
int tempData[9];
int average = -1.0;

int n;

const int sendDataLength = 8;               // length of the sendData array
char sendData[sendDataLength];              // array of that we will be sending to the LCD

int lcdFlag = 0;


float ADCtoTemp(int);
char checkKeypad(void);
int movingAverage(unsigned int);
void populateSendData(float, int);
int sendToLCD(void);


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

    UCB0CTLW1 |= UCASTP_2;   // auto STOP mode
    UCB0TBCNT |= 1;         // count = 1 byte

    //-- setup ports
    P1SEL1 &= ~BIT3;   //P1.3 SCL (48)
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;   //P1.2 SDA (1)
    P1SEL0 |= BIT2;


    //Setup ADC

    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;   // take B0 out of SW RST

    //Config ADC
    ADCCTL0 &= ~ADCSHT;     //clear ADSHT from def. of ADCSHT=01
    ADCCTL0 |= ADCSHT_2;    //16 cycles;
    ADCCTL0 |= ADCON;       //ADC on

    ADCCTL1 |= ADCSSEL_2;   //ADC clock = SMCLK
    ADCCTL1 |= ADCSHP;      //sample source = sample timer
    ADCCTL2 &= ~ADCRES;     //clear ADCRES from def. of ADCRES=01
    ADCCTL2 |= ADCRES_2;    //10bit resolution
    ADCMCTL0 |= ADCINCH_11;  //ADC input channel = A9

    ADCIE |= ADCIE0;

    P5SEL1 |= BIT3;         //P5.3 (#40)
    P5SEL0 |= BIT3;

    //P5DIR |= BIT3;
    //P5OUT &= ~BIT3;

    /*
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
    */

    //enable B0 TX0 IRQ
    UCB0IE |= UCTXIE0;  //local enable for TX0
    __enable_interrupt();   //enable maskables



    int i;

    while(1){

        ADCCTL0 |= ADCENC | ADCSC;      //Enable and Start conversion
        //while((ADCIFG & ADCIFG0) == 0); //waits until conversion is done

        keypad = checkKeypad();
        keypad = 'B';
        n = 3;

        //if(keypad != 0x00 && keypad != prevInput){
            if(keypad == '*'){
                resetTempData();
                sendData[0] = 'A';
                UCB0I2CSA = 0x02;       // set slave address LCD
                UCB0CTLW0 |= UCTXSTT;   //manually start
                resetTempData();
                for(i=0; i<100; i=i+1){}
            }else if((keypad == 'A' || keypad == 'B' || keypad == 'C' || keypad == 'D') && (keypad != '0x00' && keypad != prevInput)){
                UCB0I2CSA = 0x01;       // set slave address LED
                UCB0CTLW0 |= UCTXSTT;   //manually start
                for(i=0; i<100; i=i+1){}
            }else{

                if(average == -1.0){
                    average = movingAverage(n);
                }else{
                    average = movingAverage(n);
                    tempC = (float)(1.8641 - ((float)average*0.00080859))/0.01171;
                    tempK = tempC + 273;
                    populateSendData(tempC, tempK);
                    sendToLCD();
                    lcdFlag = 0;
                }

            }

        //}


        prevInput = keypad;

    }
    return 0;

}

float ADCtoTemp(int ADCvalue){
    return (float)(1.8641 - ((float)ADCvalue*0.00080859))/0.01171;
    /*
    float vOut;
    vOut = (3.3)*(ADCvalue/4096.0f);

    tempC = (sqrtf((2196200)+((1.8639-vOut)/(0.00000388))) - 1481.96);
    tempK = tempC + 273;
    */
}

int movingAverage(unsigned int averageNum){
    if(averageNum == 0){
        return 0.0;
    }else if(tempData[averageNum - 1] == 0){
        return -1.0;
    }

    int index;
    int avg = 0;

    for(index = 0; index < averageNum; index++){
        avg += tempData[averageNum - 1];
    }

    //avg = avg/averageNum;
    return avg/averageNum;
    /*
    float vOut;
    avg = avg/4096;
    avg = 3.3 * avg;

    vOut = (3.3)*(avg/4096.0f);

    tempC = (sqrtf((2196200)+((1.8639-vOut)/(0.00000388))) - 1481.96);
    return  tempC + 273;
    //avg = avg /averageNum;

    //return (3.3 * (avg/4096.0f));
     */
}

int sendToLCD(){
    int i;

    for(globalIndex = 0; globalIndex < sendDataLength; globalIndex++){
        lcdFlag = 1;
        UCB0I2CSA = 0x02;       // set slave address to LCD
        UCB0CTLW0 |= UCTXSTT;   //manually start
        for(i=0; i<5000; i=i+1){}
    }
    //for(i=0; i<5000; i=i+1){}

}

void populateSendData(float tempC, int tempK) {

    int index;

    float fullValue = tempC * 10;                               // since a decimal number, make it a full number: 13.5 -> 135
    for(index = sendDataLength - 1; index > 2; index--) {


        int integerValue = (int) fullValue % 10;                // mod `fullValue` and cast to integer 135 % 10 = (int) 5
        char charValue = integerValue + '0';                 // convert new `integerValue` to char 5 -> '5'
        sendData[index] = charValue;                            // add charValue into sendData array
        fullValue /= 10;                                        // divide down number
    }

    // same process but with the kelvin value. No need to convert to full value because tempK is type [int]
    for(index = 4; index > -1; index--) {


        int integerValue = tempK % 10;
        char charValue = integerValue + '0';
        sendData[index] = charValue;
        tempK /= 10;
        if(index == 1) {                                        // if third iteration, add decimal point '.'
            sendData[index] = keypad;
            continue;
        }else if(index == 0){
            sendData[index] = '%';
        }
    }
}

char checkKeypad(void) {
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
        return 0x00;
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
        return 0x00;
    };

    char button = 0x00;

    switch(buttonPressed) {
        case 0x11:
            button = '1';
            n = 1;
            break;
        case 0x12:
            button = '2';
            n = 2;
            break;
        case 0x14:
            button = '3';
            n = 3;
            break;
        case 0x18:
            button = 'A';
            break;

        case 0x21:
            button = '4';
            n = 4;
            break;
        case 0x22:
            button = '5';
            n = 5;
            break;
        case 0x24:
            button = '6';
            n = 6;
            break;
        case 0x28:
            button = 'B';
            break;

        case 0x41:
            button = '7';
            n = 7;
            break;
        case 0x42:
            button = '8';
            n = 8;
            break;
        case 0x44:
            button = '9';
            n = 9;
            break;
        case 0x48:
            button = 'C';
            break;

        case 0x81:
            button = '*';
            break;
        case 0x82:
            button = '0';
            break;
        case 0x84:
            button = '#';
            break;
        case 0x88:
            button = 'D';
            break;
    }
    return button;

}

void resetTempData() {
    readIndex = 0;                                              // reset global reading index
    globalIndex = 0;

    tempData[0] = 0;                                            // reset tempData array
    tempData[1] = 0;
    tempData[2] = 0;
    tempData[3] = 0;
    tempData[4] = 0;
    tempData[5] = 0;
    tempData[6] = 0;
    tempData[7] = 0;
    tempData[8] = 0;

    sendData[0] = 0;                                            // reset tempData array
    sendData[1] = 0;
    sendData[2] = 0;
    sendData[3] = 0;
    sendData[4] = 0;
    sendData[5] = 0;
    sendData[6] = 0;
    sendData[7] = 0;
    sendData[8] = 0;
    sendData[9] = 0;
}


//--------------------------------------------
//--ISRs
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    //P5OUT |= BIT3;


    if(lcdFlag == 1){
        UCB0TXBUF = sendData[globalIndex];
    }else{
       UCB0TXBUF = keypad;
    }

}

#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void){


    if(readIndex == n){
        readIndex = 0;
        tempData[readIndex] = ADCMEM0;
        readIndex++;
    }else{
        tempData[readIndex] = ADCMEM0;
        readIndex++;
    }
    //ADCvalue = ADCMEM0;
    //ADCtoTemp(ADCvalue);

}

