#include "lcd.h"

int CursorPos = 0;
int i;

void Delay(int D){
    for (i = 0; i < D; i++) {
        //Delay
    }
}

void Latch(){
    Delay(800);
    P2OUT |= BIT6;  // Latch
    Delay(800);
    P2OUT &= ~BIT6;
    Delay(800);
}

void BitSet(int RS, int DB7, int DB6, int DB5, int DB4){
    Delay(800);
    if(RS == 1){
        P2OUT |= (BIT7); //set RS
    }
    else{
        P2OUT &= ~(BIT7); //clear RS
    }

    if(DB7){
        P1OUT |= (BIT7); //set 7
    }
    else{
        P1OUT &= ~(BIT7); //clear 7
    }
    if(DB6){
        P1OUT |= (BIT6); //set 6
    }
    else{
        P1OUT &= ~(BIT6); //clear 6
    }
    if(DB5){
        P1OUT |= (BIT5); //set 5
    }
    else{
        P1OUT &= ~(BIT5); //clear 5
    }
    if(DB4){
        P1OUT |= (BIT4); //set 4
    }
    else{
        P1OUT &= ~(BIT4); //clear 4
    }
    Latch();
}

void FunctionSet(void){
    BitSet(0,0,0,1,1);
}

void FunctionSet4Bit(void){
    BitSet(0,0,0,1,0);
}

void ClearDisplay(void){
    BitSet(0,0,0,0,0);
    BitSet(0,0,0,0,1);
}

void ClearAll(void){
    BitSet(0,0,0,0,0);
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
    BitSet(0,1,1,0,0);

//block 7
    ClearAll();

//block 8
    BitSet(0,1,0,0,0);

// block 9-10
    ClearDisplay();

//block 11
    ClearAll();

//block 12
    BitSet(0,0,1,1,0);
    Delay(800);

//turn on LCD
    ClearAll();
    BitSet(0,1,1,0,0);
    Delay(800);
}
// END INIT


//ROWS AND COLUMNS ------------------------------------------------------------

void Column2(){
    BitSet(1,0,0,1,0);
}

void Column3(){
    BitSet(1,0,0,1,1);
}

void Column4(){
    BitSet(1,0,1,0,0);
}

void Row0(){
    BitSet(1,0,0,0,0);
}

void Row1(){
    BitSet(1,0,0,0,1);
}

void Row2(){
    BitSet(1,0,0,1,0);
}

void Row3(){
    BitSet(1,0,0,1,1);
}

void Row4(){
    BitSet(1,0,1,0,0);
}

void Row5(){
    BitSet(0,0,1,0,1);
}

void Row6(){
    BitSet(1,0,1,1,0);
}

void Row7(){
    BitSet(1,0,1,1,1);
}

void Row8(){
    BitSet(1,1,0,0,0);
}

void Row9(){
    BitSet(1,1,0,0,1);
}

void Row10(){
    BitSet(1,1,0,1,0);
}


//KEYPAD --------------------------
void KeyEntered(char c) {
    P2OUT |= BIT7;     //set RSbit

    //Print entered char
    switch(c)
    {
    case 'A': Column4();
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
            break;
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
    case '6': Column3();
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
    case '*': Column2();
            Row10();
            break;
    case '#': Column2();
            Row3();
            break;
    default: Column4();
            Row1();
            break;
    }

    // cursor moves to next pos, check for 0F/4F condition
    CursorPos++;
    if (CursorPos == 16) {      //Cursor moved past 0F
        BitSet(0,1,1,0,0);
        ClearAll();
        Delay(800);
    }
    else if (CursorPos == 32) { //cursor moved to end position, clear screen and reset cursor
        CursorPos = 0;
        BitSet(0,1,0,0,0);
        ClearAll();
        Delay(800);

    //clear display
        ClearDisplay();
    }

} //-------------- END KeyEntered ------------------------------------

