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

void Column3();

void KeyEntered(char c);

#endif // LCD_H_
