#include "lcd.h"

//int CursorPos = 0;
int i;

void Delay(int D){
    for (i = 0; i < D; i++) {
        //Delay
    }
}

void Latch(){
    Delay(50);
    P2OUT |= BIT6;  // Latch
    Delay(50);
    P2OUT &= ~BIT6;
    Delay(50);
}

void BitSet(int RS, char data){
    Delay(800);
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

    //E
    BitSet(1,0x04);
    BitSet(1,0x05);

    //n
    BitSet(1,0x06);
    BitSet(1,0x0E);

    //t
    BitSet(1,0x07);
    BitSet(1,0x04);

    //e
    BitSet(1,0x06);
    BitSet(1,0x05);

    //r
    BitSet(1,0x07);
    BitSet(1,0x02);

    //set pos to 06
    BitSet(0,0x08);
    BitSet(0,0x06);

    //n
    BitSet(1,0x06);
    BitSet(1,0x0E);

    //:
    BitSet(1,0x03);
    BitSet(1,0x0A);

    //set pos to 40
    BitSet(0,0x0C);
    BitSet(0,0x00);

    //T
    BitSet(1,0x05);
    BitSet(1,0x04);

    //set pos to 42
    BitSet(0,0x0C);
    BitSet(0,0x02);

    //=
    BitSet(1,0x03);
    BitSet(1,0x0D);

    //set pos to 47
    BitSet(0,0x0C);
    BitSet(0,0x07);

    //degree
    BitSet(1,0x0D);
    BitSet(1,0x0F);

    //K
    BitSet(1,0x04);
    BitSet(1,0x0B);

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

void Column2(){
    BitSet(1,0x02);
}

void Column3(){
    BitSet(1,0x03);
}

void Column4(){
    BitSet(1,0x04);
}

void Row0(){
    BitSet(1,0x00);
}

void Row1(){
    BitSet(1,0x01);
}

void Row2(){
    BitSet(1,0x02);
}

void Row3(){
    BitSet(1,0x03);
}

void Row4(){
    BitSet(1,0x04);
}

void Row5(){
    BitSet(1,0x05);
}

void Row6(){
    BitSet(1,0x06);
}

void Row7(){
    BitSet(1,0x07);
}

void Row8(){
    BitSet(1,0x08);
}

void Row9(){
    BitSet(1,0x09);
}

void Row10(){
    BitSet(1,0x0A);
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
            Row0();
            break;
    case '1': Column3();
            Row1();
            break;
    case '2': Column3();
            Row2();
            break;
    case '3': Column3();
            Row3();
            break;
    case '4': Column3();
            Row4();
            break;
    case '5': Column3();
            Row5();
            break;
    case '6':
            Column3();
            Row6();
            break;
    case '7': Column3();
            Row7();
            break;
    case '8': Column3();
            Row8();
            break;
    case '9': Column3();
            Row9();
            break;
    case 'A': LcdTempInit();
            break;
    default: Column4();
            Row1();
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
