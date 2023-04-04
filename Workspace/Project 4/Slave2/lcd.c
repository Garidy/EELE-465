#include "lcd.h"

//int CursorPos = 0;
int i;

void Delay(int D){
    for (i = 0; i < D; i++) {
        //Delay
    }
}

void Latch(){
    Delay(5);
    P2OUT |= BIT6;  // Latch
    Delay(5);
    P2OUT &= ~BIT6;
    Delay(5);
}

void BitSet(int RS, char data){
    Delay(200);
    P2OUT &= ~(BIT7); //clear RS
    P1OUT &= ~(BIT7); //clear 7
    P1OUT &= ~(BIT6); //clear 6
    P1OUT &= ~(BIT5); //clear 5
    P1OUT &= ~(BIT4); //clear 4
    if(RS == 1){
        P2OUT |= (BIT7); //set RS
    }

    P1OUT |= (data << 4);

    Latch();
}

void FunctionSet(void){
    BitSet(0,0x03);
}

void FunctionSet4Bit(void){
    BitSet(0,0x02);
}

void ClearDisplay(void){
    BitSet(0,0x00);
    BitSet(0,0x01);
}

void ClearAll(void){
    BitSet(0,0x00);
}

void LcdInit(void){

//block 1-3 function set
    Delay(1000);
    FunctionSet();
    FunctionSet();
    FunctionSet();

//block 4-5 function set 4 bit mode
    FunctionSet4Bit();
    FunctionSet4Bit();

//block 6
    BitSet(0,0x0C);

//block 7
    ClearAll();

//block 8
    BitSet(0,0x08);

// block 9-10
    ClearDisplay();

//block 11
    ClearAll();

//block 12
    BitSet(0,0x06);
    Delay(800);

//turn on LCD
    ClearAll();
    BitSet(0,0x0C);
    Delay(800);
}
// END INIT

void LcdTempInit(void){
    ClearDisplay();

    //set cursor pos to 0
    BitSet(0,0x08);
    BitSet(0,0x00);

    //R
    BitSet(1,0x05);
    BitSet(1,0x02);

    //e
    BitSet(1,0x06);
    BitSet(1,0x05);

    //s
    BitSet(1,0x07);
    BitSet(1,0x03);

    //=
    BitSet(1,0x03);
    BitSet(1,0x0D);

    //set pos to 08
    BitSet(0,0x08);
    BitSet(0,0x08);

    //A
    BitSet(1,0x04);
    BitSet(1,0x01);

    //:
    BitSet(1,0x03);
    BitSet(1,0x0A);

    //set pos to 0E
    BitSet(0,0x08);
    BitSet(0,0x0E);

    //degree
    BitSet(1,0x0D);
    BitSet(1,0x0F);

    //C
    BitSet(1,0x04);
    BitSet(1,0x03);

    //M
    BitSet(1,0x04);
    BitSet(1,0x0D);

    //:
    BitSet(1,0x03);
    BitSet(1,0x0A);

    //set pos to 48
    BitSet(0,0x0C);
    BitSet(0,0x08);

    //P
    BitSet(1,0x05);
    BitSet(1,0x00);

    //:
    BitSet(1,0x03);
    BitSet(1,0x0A);

    //set pos to 4E
    BitSet(0,0x0C);
    BitSet(0,0x0E);

    //degree
    BitSet(1,0x0D);
    BitSet(1,0x0F);

    //C
    BitSet(1,0x04);
    BitSet(1,0x03);

}

//ROWS AND COLUMNS ------------------------------------------------------------

void Column3(){
    BitSet(1,0x03);
}




//KEYPAD --------------------------
void KeyEntered(char c) {
    P2OUT |= BIT7;     //set RSbit

    //Print entered char
    switch(c)
    {
/*    case 'A': Column4();
            Row1();
            break;
    case 'B': Column4();
            Row2();
            break;
    case 'C': Column4();
            Row3();
            break;
    case 'D': Column4();
            Row4();
            break; */
    case '0': Column3();
            BitSet(1,0x00);
            break;
    case '1': Column3();
            BitSet(1,0x01);
            break;
    case '2': Column3();
            BitSet(1,0x02);
            break;
    case '3': Column3();
            BitSet(1,0x03);
            break;
    case '4': Column3();
            BitSet(1,0x04);
            break;
    case '5': Column3();
            BitSet(1,0x05);
            break;
    case '6': Column3();
            BitSet(1,0x06);
            break;
    case '7': Column3();
            BitSet(1,0x07);
            break;
    case '8': Column3();
            BitSet(1,0x08);
            break;
    case '9': Column3();
            BitSet(1,0x09);
            break;
    case '*': LcdTempInit();
            break;
    default: Column3();
            BitSet(1,0x02);
            break;
    }

 /*   // cursor moves to next pos, check for 0F/4F condition
    CursorPos++;
    if (CursorPos == 16) {      //Cursor moved past 0F
        BitSet(0,0x0A);
        ClearAll();
        Delay(800);
    }
    else if (CursorPos == 32) { //cursor moved to end position, clear screen and reset cursor
        CursorPos = 0;
        BitSet(0,0x08);
        ClearAll();
        Delay(800);

    //clear display
        ClearDisplay();
    } */

} //-------------- END KeyEntered ------------------------------------
