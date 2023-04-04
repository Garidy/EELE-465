#include <msp430fr2310.h>
#ifndef LCD_H_   /* Include guard */
#define LCD_H_



void LcdInit(void);
void LcdTempInit(void);
void Latch();
void FunctionSet(void);
void FunctionSet4Bit(void);
void ClearDisplay(void);
void ClearAll(void);
void BitSet(int RS, char data);

void Delay(int D);

void Column1();
void Column2();
void Column3();
void Column4();

void Row0();
void Row1();
void Row2();
void Row3();
void Row4();
void Row5();
void Row6();
void Row7();
void Row8();
void Row9();
void Row10();

void KeyEntered(char c);

#endif // LCD_H_
