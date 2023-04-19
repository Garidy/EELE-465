#include <msp430.h> 

// constants
const char slaveAddrLED = 0x01;            // LED address
const char slaveAddrLCD = 0x02;            // LCD address
const char slaveAddrRTC = 0x068;            // RTC address
const char slaveAddrLM92 = 0x048;           // LM92 address
const int sendDataLength = 12;               // length of the sendData array
const int refreshSeconds = 2;                // LCD refresh rate in seconds


// global counters
unsigned int readIndexLM19 = 0;             // global variable to populate tempDataLM19 array
unsigned int readIndexLM92 = 0;             // global variable to populate tempDataLM92 array
int timerCount = 0;                         // global timer count to detect 0.5s, 0.33s, and 1s
unsigned int globalIndex;                   // global index to send in I2C ISR


// LCD variables
int isLCDdata = 1;                          // 0 is YES, 1 is NO
unsigned int LCDrefresh = 1;                // LCD refresh flag


// LMxx variables
int LM92data;                               // initial data we get from I2C
unsigned int LM92refresh = 1;               // 0 is YES, 1 is NO
int tempDataLM19[9];                        // array where each ADC value reading from LM19 is stored
int tempDataLM92[9];                        // array where each ADC value reading from LM92 is stored
char sendData[sendDataLength];              // array that we will be sending to the LCD
unsigned int LM92DataCount = 0;
char LM92dataUpper, LM92dataLower;


// LED variables
int patternCount = 1;                       // keep track of LED patterns
char LEDout = 0b01010101;                                // char to send to LED
unsigned int LEDrefresh = 1;                // LED flag


// keypad variables
unsigned int keypadInt = 0;                 // most recent number keypad value (pressed)
char keypadChar = 'D';                      // most recent letter keypad value (pressed)
char sendChar;                              // character sent to RTC (register) or LED (keypad letter)


// RTC variables
char minutes;                               // minutes - encoded in hex but value is int
char seconds;                               // seconds - encoded in hex but value is int
unsigned int rtcDataCount = 0;              // start counter at 0
unsigned int getRTCdata = 0;                // get RTC data flag


// program functions
void getTimeFromRTC();
float movingAverage(unsigned int, int);
void populateSendData(int, float, float, int);
void sendToLCD(char);
int getRTCseconds();
void getTempFromLM92();
void LEDpattern();
void sendToLED();
float LM92conversion(int);

void HeatPlant(void);
void CoolPlant(void);
void PlantOff(void);


// helper functions
char checkKeypad();
int delay(int);
int _delay1ms();
char _getCharacter(char);
int _toInt(char);
char _toChar(int);
float _adcToTempC(float);
char _hexToChar(char);

/**
 * Still Missing:
 * - LM19 temp correct value
 */


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer

     // Setup I2C ports
    UCB0CTLW0 |= UCSWRST;               // put into software reset

    UCB0CTLW0 |= UCSSEL_3;              // choose SMCLK
    UCB0BRW = 10;                       // divide by 10 to get SCL = 100kHz

    UCB0CTLW0 |= UCMODE_3;              // put into I2C mode
    UCB0CTLW0 |= UCMST;                 // put into Master mode
    UCB0CTLW0 |= UCTR;                  // put into Tx/WRITE mode

    UCB0CTLW1 |= UCASTP_2;              // AUTO stop when UCB0CNT reached
    UCB0TBCNT = 0x01;                   // send 1 byte of data

    //-- setup ports
    P1SEL1 &= ~BIT3;   //P1.3 SCL (48)
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;   //P1.2 SDA (1)
    P1SEL0 |= BIT2;

    //Setup Timer compare
    //divide by 4 and 16 bit with SMClock
    //triggers about 3 times a second
    TB0CTL |= TBCLR;                    //Clear timers and dividers
    TB0CTL |= TBSSEL__ACLK;             // ACLK
    TB0CTL |= MC__UP;                   //count up to value in TB0CCR0
    TB0CTL |= CNTL_1;                   //use 12-bit counter
    TB0CTL |= ID__8;                    //use divide by 8

    TB0CCR0 = 0x2AA;                    // count up to approx 332ms (read values 3 times a second)
    TB0CCTL0 &= ~CCIFG;                 //Clear Flag


    // configure ADC ports
    P5SEL1 |= BIT3;         //P5.3 (#40)
    P5SEL0 |= BIT3;


    //ADD THIS --------------
    P3DIR |= BIT0;   //P3.0 COOL (47)
    P3OUT &= ~BIT0;

    P3DIR |= BIT1;   //P3.1 HEAT (46)
    P3OUT &= ~BIT1;
    //-----------------------



    UCB0CTLW0 &= ~UCSWRST;               // put out of software reset
    PM5CTL0 &= ~LOCKLPM5;                // Turn on I/O

    // configure ADC
    ADCCTL0 &= ~ADCSHT;                  // clear ADCSHT from def. of ADCSHT=01
    ADCCTL0 |= ADCSHT_2;                 // conversion cycles = 16 (ADCSHT=10)
    ADCCTL0 |= ADCON;                    // turn ADC on

    ADCCTL1 |= ADCSSEL_2;                // ADC clock source = SMCLK
    ADCCTL1 |= ADCSHP;                   // sample signal source = sampling timer
    ADCCTL2 &= ~ADCRES;                  // Clear ADCRES from def. of ADCRES=01
    ADCCTL2 |= ADCRES_2;                 // resolution = 12-bit (ADCRES=10)
    ADCMCTL0 |= ADCINCH_11;               // ADC input channel = A2 (P1.2)

    UCB0IE |= UCTXIE0;                   // Enable I2C TX0 IRQ (transmit reg is ready)
    UCB0IE |= UCRXIE0;                   // Enable I2C RX0 IRQ (receive reg is ready)
    TB0CCTL0 |= CCIE;                    // local enable timer interrupt
    __enable_interrupt();                // global enable

    P5DIR |= BIT1;                       // output P5.2 (Peltier HOT)
    P5DIR |= BIT2;                       // output P5.3 (Peltier COLD)

    P5OUT &= ~BIT1;                      // start P5.2 as OFF (HOT)
    P5OUT &= ~BIT2;                      // start P5.3 as OFF (COLD)

    // variables to populate sendData
    float averageLM19 = -1.0;                         // initialize average variable for LM19
    float averageLM92 = -1.0;                         // initialize average variable for LM92
    float LM19tempC, LM92tempC;


    // testing loop here
    while(1) {
        int i;

        int currentTimeSeconds = getRTCseconds();

        if(LM92refresh == 0) {
            getTempFromLM92();                                      // acquire temperature from LM92
            averageLM92 = movingAverage(keypadInt, 2);              // calculate the moving average LM92 based on the num pressed
            LM92tempC = LM92conversion(averageLM92);                // convert average LM92 value to temperature
        }

        if(getRTCdata == 1) {                                       // timer reached 1s, start getting RTC data
            getTimeFromRTC();                                       // populate RTC time in global variables `minutes` and `seconds`
        }

        if(LCDrefresh == 2 && keypadInt != 0) {                     // refresh if it's been 2 seconds and keypad 0 is not pressed
            sendToLCD('n');                                         // send `sendData[]` to LCD (no refresh)
        }



        if(keypadInt != 0) {                                        // if an average number was identified, calculate moving average
            while(averageLM19 == -1.0) {
                averageLM19 = movingAverage(keypadInt, 1);                              // calculate the moving average of LM19 based on the num pressed
            }

            LM19tempC = _adcToTempC(averageLM19);                                  // calculate temp in celsius based on ADC value

            populateSendData(keypadInt, LM19tempC, LM92tempC, currentTimeSeconds);      // populate sendData array for I2C transmission
        }


        char keypad = checkKeypad();              // check keypad value. Will return '\0' if nothing

        if(keypad != '\0') {                      // if a button was selected during this loop cycle

            // If a letter was pressed, send signals to the peltier
            if(keypad == 'A' || keypad == 'B' || keypad == 'C' || keypad == 'D') {
                keypadChar = keypad;

                if(keypadChar == 'A') {                 // start heating
                    P3OUT &= ~BIT0;
                    delay(10);
                    P3OUT |= BIT1;
                }
                else if(keypadChar == 'B') {            // start cooling
                    P3OUT &= ~BIT1;
                    delay(10);
                    P3OUT |= BIT0;
                }
                else if(keypadChar == 'C'){
                    if (LM92tempC > (LM19tempC + 2)){   // if plant > ambient + 2, then cool
                        P3OUT &= ~BIT1;
                        delay(10);
                        P3OUT |= BIT0;
                    }
                    else if (LM92tempC < (LM19tempC - 2)){   // if plant < ambient - 2, then heat
                        P3OUT &= ~BIT0;
                        delay(10);
                        P3OUT |= BIT1;
                    }
                    else {          // else turn off plant
                        P3OUT &= ~BIT1;
                        P3OUT &= ~BIT0;
                    }
                }
                else if(keypadChar == 'D') {            // turn off fan
                    P3OUT &= ~BIT1;
                    P3OUT &= ~BIT0;
                }

                sendToLED();
                delay(10);

            }
            else if(keypad == '*') {
                UCB0I2CSA = slaveAddrLCD;                               // slave address to 0x12 (LED slave address)
                sendChar = 'A';                                      // `LEDout` will be sent to LED
                UCB0CTLW0 |= UCTR;                                      // Put into Tx mode
                UCB0TBCNT = 0x01;                                       // Send 1 byte of data at a time
                UCB0CTLW0 |= UCTXSTT;                                   // Generate START, triggers ISR right after

                for(i=0; i<100; i=i+1){}


            }

            else if(keypad == '#') {}  // do nothing

            else {                                      // if a number was pressed
                keypadInt = _toInt(keypad);             // update global variable to newest keypad press
            }

            LM92tempC = 22.2;
            LM19tempC = 33.3;
            currentTimeSeconds = 444;


            populateSendData(keypadInt, LM19tempC, LM92tempC, currentTimeSeconds);          // populate sendData array for I2C transmission

            sendToLCD('y');                                                                 // send reset character to LCD
            timerCount = 0;                                                                 // reset timer counter
            sendToLCD('n');                                                                 // send currently populated sendData
            delay(5);                                                                       // small delay to pick up keypad presses

        }
    }

    return 0;
}

/**
 * Function that initiates communication to LED
 * LED address is 0x0012
 * Will send the value in `LEDout` (determined in `LEDpattern()`)
 */
void sendToLED() {
    int i;

    UCB0I2CSA = slaveAddrLED;                               // slave address to 0x12 (LED slave address)
    sendChar = keypadChar;                                      // `LEDout` will be sent to LED
    UCB0CTLW0 |= UCTR;                                      // Put into Tx mode
    UCB0TBCNT = 0x01;                                       // Send 1 byte of data at a time
    UCB0CTLW0 |= UCTXSTT;                                   // Generate START, triggers ISR right after
    for(i=0; i<100; i=i+1){}
    UCB0IFG &= ~UCSTPIFG;

}

/**
 * Grabs the minute and second (encoded in hex) and returns the value in seconds
 */
int getRTCseconds() {
    char bottomNibble, topNibble;
    char s = seconds;

    bottomNibble = (s << 4);                                                                // left shift to remove top nibble
    bottomNibble = bottomNibble >> 4;                                                       // right shift to get correct value
    topNibble = 0x00 | s >> 4;                                                              // grab top nibble
    int secondsInt = _toInt(_hexToChar(topNibble)) * 10 + _toInt(_hexToChar(bottomNibble)); // compute seconds

    s = minutes;
    bottomNibble = (s << 4);
    bottomNibble = bottomNibble >> 4;
    topNibble = 0x00 | s >> 4;
    int minutesInt = _toInt(_hexToChar(topNibble)) * 10 + _toInt(_hexToChar(bottomNibble)); // compute minutes


    return minutesInt * 60 + secondsInt;                                                    // return time in seconds
}

/**
 * Computes the value to send to the LED
 * `LEDout` value will be sent to LED
 */
void LEDpattern() {
    int tempVar;                            // temporary variable

    //alternates between 2 led patterns when not cooling or heating
    if(keypadChar == 'D') {

        if(patternCount == 0) {
            LEDout = 0b01010101;
            patternCount = 1;
        }
        else if(patternCount == 1) {
            LEDout = 0b10101010;
            patternCount = 0;
        }
    }

    else if(keypadChar == 'B') {            // pattern for cooling

        if (patternCount == 9) {
            LEDout = 0;
            patternCount = 0;
        }
        else if(patternCount == 1) {
            LEDout = 0b10000000;
        }
        else {
            tempVar = LEDout >> 1;          // rotates the bits and adds one to the end so scrolling 1's MSB to LSB
            LEDout = tempVar | LEDout;
        }

        patternCount++;
    }

    else if(keypadChar == 'A') {            // pattern for heating

        if (patternCount == 9) {
            LEDout = 0;
            patternCount = 0;
        }
        else if(patternCount == 1) {
            LEDout = 0b00000001;
        }
        else {
            tempVar = LEDout << 1;         // same as B but from LSB to MSB
            LEDout = tempVar | LEDout;
        }

        patternCount++;
    }
}

/**
 * Converts values to char
 * Populates sendData array with chars
 * Will be sending: keypadInt, keypadChar, RTC number of seconds, tempLM19, tempLM92
 * Example array: [ '5', '13.4',  'A', '290', '29.1']
 * -> ['5', '1', '3', '.', '4', 'A', '2', '9', '0', '2', '9', '.', '1']
 */

// ['%', 'N', 'A','A','A', 'M', 'T','T','T', 'P','P','P']
//  0     1    2   3   4    5    6   7   8    9  10  11
// ['%', '9', '2','1','5', 'A', '0','2','4', '2','5','0']
void populateSendData(int kInt, float tempLM19, float tempLM92, int RTCseconds) {
    int index;

    // position 0
    sendData[0] = '%';
    sendData[1] = _toChar(kInt);

    // from position 2 -> 4
    float fullValueFloat = tempLM19 * 10;
    int fullValue = (int) fullValueFloat;                               // since a decimal number, make it a full number: 13.5 -> 135

    for(index = 4; index > 1; index--) {
        int integerValue = (int) fullValue % 10;                // mod `fullValue` and cast to integer 135 % 10 = (int) 5
        char charValue = _toChar(integerValue);                 // convert new `integerValue` to char 5 -> '5'
        sendData[index] = charValue;                            // add charValue into sendData array
        fullValue /= 10;                                        // divide down number
    }

    // position 5
    sendData[5] = keypadChar;

    // from position 6 -> 8
    for(index = 8; index > 5; index--) {

        int integerValue = (int) RTCseconds % 10;                // mod `fullValue` and cast to integer 135 % 10 = (int) 5
        char charValue = _toChar(integerValue);                 // convert new `integerValue` to char 5 -> '5'
        sendData[index] = charValue;                            // add charValue into sendData array
        RTCseconds /= 10;                                        // divide down number
    }

    // from position 9 -> 11
    fullValueFloat = tempLM92 * 10;
    fullValue = (int) fullValueFloat;
    for(index = 11; index > 8; index--) {

        int integerValue = (int) fullValue % 10;                // mod `fullValue` and cast to integer 135 % 10 = (int) 5
        char charValue = _toChar(integerValue);                 // convert new `integerValue` to char 5 -> '5'
        sendData[index] = charValue;                            // add charValue into sendData array
        fullValue /= 10;                                        // divide down number
    }
}


/**
 * Generates start condition to LCD device
 * Send contents in sendData[]
 * refresh = 'y' -> send refresh bit to LCD
 */
void sendToLCD(char refresh) {
    int i;
    /*
    if(refresh == 'y') {
        UCB0I2CSA = slaveAddrLCD;                               // slave address to 0x14 (LCD slave address)
        sendChar = '5';                                         // '*' will be sent to LCD
        UCB0CTLW0 |= UCTR;                                      // Put into Tx mode
        UCB0TBCNT = 0x01;                                       // Send 1 byte of data at a time
        UCB0CTLW0 |= UCTXSTT;                                   // Generate START, triggers ISR right after
        for(i=0; i<100; i=i+1){}

        //while((UCB0IFG & UCSTPIFG) == 0) {}                     // Wait for START, S.Addr, & S.ACK (Reg UCB0IFG contains UCSTPIFG bit)
        UCB0IFG &= ~UCSTPIFG;
        return;
    }
    */

    isLCDdata = 0;                                              // enable LCD data transmission

    for(globalIndex = 0; globalIndex < sendDataLength; globalIndex++) {
        UCB0I2CSA = slaveAddrLCD;                               // slave address to 0x14 (LCD slave address)
        UCB0CTLW0 |= UCTR;                                      // Put into Tx mode
        UCB0TBCNT = 0x01;                                       // Send 1 byte of data at a time
        UCB0CTLW0 |= UCTXSTT;                                   // Generate START, triggers ISR right after

        for(i=0; i<100; i=i+1){}

        //while((UCB0IFG & UCSTPIFG) == 0) {}                     // Wait for START, S.Addr, & S.ACK (Reg UCB0IFG contains UCSTPIFG bit)
        UCB0IFG &= ~UCSTPIFG;                                   // Clear flag
    }

    isLCDdata = 1;                                              // disable LCD data transmission flag
}

//converts LM92 raw data to Celsius
float LM92conversion(int LM92dataFromI2C){
    float final;         //initialize a float
    LM92dataFromI2C = LM92dataFromI2C & ~(BIT0 | BIT1 | BIT2); //clear off the status bits
    LM92dataFromI2C = LM92dataFromI2C >> 3;            //move the binary number to the LSB
    final = LM92dataFromI2C * .0625;            //multiply by .0625 to get the celsius number
    return final;
}

/**
 * Get's current temp value from LM92
 */
void getTempFromLM92() {
    int i;
    UCB0I2CSA = slaveAddrLM92;                 // Set Slave Address to LM92 -> 0x0048
    UCB0CTLW0 &= ~UCTR;                        // Put into Rx mode
    UCB0TBCNT = 0x02;                          // Send 1 byte of data at a time
    UCB0CTLW0 |= UCTXSTT;                      // Generate START
    for(i=0; i<100; i=i+1){}

   // while((UCB0IFG & UCSTPIFG) == 0) {}        // wait for stop condition
    UCB0IFG &= ~UCSTPIFG;                      // Clear flag

    LM92data = (LM92dataUpper << 8) | LM92dataLower;

    if(readIndexLM92 == keypadInt) {                // reset index if limit is exceeded
        readIndexLM92 = 0;
    }

    tempDataLM92[readIndexLM92] = LM92data;         // put the data received from I2C into array

    readIndexLM92++;                                // increment readIndex for next position

    LM92refresh = 1;                                // turn off LM92 data fetching
}

/**
 * Function that gets time data from RTC
 * This will only occur every second.
 * It is dependent from the `getRTCdata` flag.
 * Flag gets toggled in `ISR_TB0_CCR0()` ISR
 */
void getTimeFromRTC() {
     int i;
     // Request time in [ss/mm] format
     UCB0I2CSA = slaveAddrRTC;                  // Set Slave Address
     UCB0CTLW0 |= UCTR;                         // Put into Tx mode
     UCB0TBCNT = 0x01;                          // Send 1 byte of data at a time
     sendChar = 0x00;                           // send RTC register to start the read from
     UCB0CTLW0 |= UCTXSTT;                      // Generate START

     for(i=0; i<100; i=i+1){}

     //while((UCB0IFG & UCSTPIFG) == 0) {}        // Wait for START, S.Addr, & S.ACK (Reg UCB0IFG contains UCSTPIFG bit)
     UCB0IFG &= ~UCSTPIFG;                      // Clear flag


     // Receive Requested Data from RTC
     UCB0I2CSA = slaveAddrRTC;                  // Set Slave Address
     UCB0CTLW0 &= ~UCTR;                        // Put into Rx mode
     UCB0TBCNT = 0x02;                          // Receive 2 bytes of data at a time
     UCB0CTLW0 |= UCTXSTT;                      // Generate START

     for(i=0; i<100; i=i+1){}

     //while((UCB0IFG & UCSTPIFG) == 0) {}
     UCB0IFG &= ~UCSTPIFG;                      // Clear flag

     getRTCdata = 0;                            // stop getting data from RTC
}


/**
 * Calculates the moving average and populates either tempDataLM19 or tempDataLM92
 * @params `averageNum` refers to the number of the moving average
 * @params `sensor` depicts which array to populate: either '19' or '92'
 * GOAL: populate the tempDataLMxx array
 *
 * Steps:
 *  tempDataLMxx[9] -> upon initialization, they all start with 0s.
 *  tempDataLMxx is populated with the timer interrupt.
 *  every second it will populate the tempDataLMxx array within
 */
float movingAverage(unsigned int averageNum, int sensor) {
//    a = averageNum;

    // if tempData is not fully populated yet, return back to main
    // prevent index out of bounds error
    if(averageNum == 0) return 0.0;

    if(sensor == 1) {
        if(tempDataLM19[averageNum - 1] == 0) return -1.0;
    }
    else if(sensor == 2) {
        if(tempDataLM92[averageNum - 1] == 0) return -1.0;
    }

    int index;
    float average = 0;

    if(sensor == 1) {
        // if tempDataLM19 is populated, add up all the values within array
        for(index = 0; index < averageNum; index++) {
            average += tempDataLM19[averageNum - 1];
        }
    }
    else if(sensor == 2) {
        // if tempDataLM92 is populated, add up all the values within array
        for(index = 0; index < averageNum; index++) {
            average += tempDataLM92[averageNum - 1];
        }
    }

    return average / averageNum;
}

/**
 * Helper functions:
 */

// Converts char to int
int _toInt(char c) {
    return c - '0';
}

// Converts int to char
char _toChar(int i) {
    return i + '0';
}

// Converts celcius to Kelvin
int _toKelvin(float tempC) {
    return tempC + 273;

}

float _adcToTempC(float averageADCvalue) {
//    return (-0.284 * averageADCvalue) + 289.89 + 4;
//    return -0.0704 * averageADCvalue + 158.11;
    return -0.0704 * averageADCvalue + 287.89;
}

char _hexToChar(char hex) {
    switch(hex) {
        case 0x01:
            return '1';
        case 0x02:
            return '2';
        case 0x03:
            return '3';
        case 0x04:
            return '4';
        case 0x05:
            return '5';
        case 0x06:
            return '6';
        case 0x07:
            return '7';
        case 0x08:
            return '8';
        case 0x09:
            return '9';
        default:
            return '0';
    }
}


// checks for the button pressed on keypad
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
    }

    return _getCharacter(buttonPressed);
}

// translates the button pressed to ASCII
char _getCharacter(char buttonPressed) {
    char button;

    switch(buttonPressed) {
        case 0x11:
            button = '1';
            break;
        case 0x12:
            button = '2';
            break;
        case 0x14:
            button = '3';
            break;
        case 0x18:
            button = 'A';
            break;

        case 0x21:
            button = '4';
            break;
        case 0x22:
            button = '5';
            break;
        case 0x24:
            button = '6';
            break;
        case 0x28:
            button = 'B';
            break;

        case 0x41:
            button = '7';
            break;
        case 0x42:
            button = '8';
            break;
        case 0x44:
            button = '9';
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
        default:
            button = '\0';
            break;
    }
    return button;
}

/**
 * delay: Delay set for approx 1 ms
 */
int delay(int LongCount) {
    while(LongCount > 0) {
        LongCount--;
        _delay1ms();
    }
    return 0;
}

/**
 * Delay1ms: Delay set for approx 1 ms
 */
int _delay1ms() {
    int ShortCount;
    for(ShortCount = 0; ShortCount < 102; ShortCount++) {}
    return 0;
}


//ADD THIS-----------------------
void HeatPlant() {
    int i;
    P3OUT &= ~BIT0; //Ensure Cool function disabled
    for(i=0; i<500; i=i+1){}    //Short Delay
    P3OUT |= BIT1; //Turn on Heat
}

void CoolPlant() {
    int i;
    P3OUT &= ~BIT1; //Ensure Heat function disabled
    for(i=0; i<500; i=i+1){}    //Short Delay
    P3OUT |= BIT0; //Turn on Cool
}

void PlantOff() {
    P3OUT &= ~BIT0; //Turn both Heat and Cool Off
    P3OUT &= ~BIT1;
}

//--------------------------------------------

// ISRs -------------------------------------------------------------
/**
 * I2C Interrupt Service Routine
 */
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) {

    switch(UCB0IV) {
        case 0x16:                                              // ID16 = RXIFG0 -> Receive
            if(LM92refresh == 0) {

                if(LM92DataCount == 0) {
                    LM92dataUpper = UCB0RXBUF;
                    LM92DataCount++;
                }
                else if(LM92DataCount == 1) {
                    LM92dataLower = UCB0RXBUF;
                     LM92DataCount = 0;
                }
            }

            else if(rtcDataCount == 0) {                        // getting seconds
                seconds = UCB0RXBUF;
                rtcDataCount++;
            }
            else if(rtcDataCount == 1) {
                minutes = UCB0RXBUF;                            // getting minutes
                rtcDataCount = 0;
            }
            break;

        case 0x18:                                              // ID18 = TXIFG0 -> Transmit
            if(isLCDdata == 0) {
                UCB0TXBUF = sendData[globalIndex];              // send current sendData char to LCD
            }
            else {
                UCB0TXBUF = sendChar;                           // send RTC register value + LED value
            }


            break;

        default:
            break;
      }

}

/**
 * Timer Interrupt Service Routine
 * Takes in ADC value from LM-19 and populates tempData array
 * Fires every 333 ms
 */
#pragma vector=TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void) {

    if(timerCount == 3 || timerCount == 6) {        // every 0.5s, grab temp values
        ADCCTL0 |= ADCENC | ADCSC;                  // grab LM19 Temp data
        while((ADCIFG & ADCIFG0) == 0);             // wait to receive all data

        if(keypadInt == 0) {
            // don't populate / do nothing
        }

        else if(readIndexLM19 == keypadInt) {           // once the amount of desired data points is obtained
            readIndexLM19 = 0;                          // reset index
            tempDataLM19[readIndexLM19] = ADCMEM0;      // start overriding values in tempDataLM19
        }
        else {
            tempDataLM19[readIndexLM19] = ADCMEM0;      // populate tempDataLM19
        }

        readIndexLM19++;


        LM92refresh = 0;                            // enable flag to get LM92 data in main subroutine
    }


    if(timerCount == 6) {                           // if 1.0s, get time data from rtc
        timerCount = -1;                             // reset timerCount
        getRTCdata = 1;                             // start getting RTC data in `main()`

        if(LCDrefresh == refreshSeconds) {
            LCDrefresh = 1;                         // restart 2 second count
        }
        else {
            LCDrefresh++;                           // count up seconds to refresh LCD
        }

    }

    timerCount++;                                   // increment global timer variable

    TB0CCTL0 &= ~CCIFG;                             // Clear Flag
}
