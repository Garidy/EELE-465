;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
; J.Wurst and G.Sanders, EELE465, Project 03
; Feb 14, 2023
; Interface with a keypad
;
;	Registers
;
;
;	Variables
;	Rx:
;
;
;	Sub Routines
;
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
init:

		;D1 = P1.3
		bic.b	#BIT3, &P1SEL0
		bic.b	#BIT3, &P1SEL1				; set as digital I/O port
		bic.b	#BIT3, &P1DIR				; set P1.3 as an input

		;D2 = P3.4
		bic.b	#BIT4, &P3SEL0
		bic.b	#BIT4, &P3SEL1				; set as digital I/O port
		bic.b	#BIT4, &P3DIR				; set P3.4 as an input

		;D3 = P4.5
		bic.b	#BIT5, &P4SEL0
		bic.b	#BIT5, &P4SEL1				; set as digital I/O port
		bic.b	#BIT5, &P4DIR				; set P4.5 as an input

		;D4 = P5.2
		bic.b	#BIT2, &P5SEL0
		bic.b	#BIT2, &P5SEL1				; set as digital I/O port
		bic.b	#BIT2, &P5DIR				; set P5.2 as an input

		;D5 = P3.6
		bic.b	#BIT6, &P3SEL0
		bic.b	#BIT6, &P3SEL1				; set as digital I/O port
		bic.b	#BIT6, &P3DIR				; set P3.6 as an input

		;D6 = P1.7
		bic.b	#BIT7, &P1SEL0
		bic.b	#BIT7, &P1SEL1				; set as digital I/O port
		bic.b	#BIT7, &P1DIR				; set P1.7 as an input

		;D7 = P1.6
		bic.b	#BIT6, &P1SEL0
		bic.b	#BIT6, &P1SEL1				; set as digital I/O port
		bic.b	#BIT6, &P1DIR				; set P1.6 as an input

		;D8 = P1.5
		bic.b	#BIT5, &P1SEL0
		bic.b	#BIT5, &P1SEL1				; set as digital I/O port
		bic.b	#BIT5, &P1DIR				; set P1.5 as an input

		;LEDs
		bic.b	#BIT0, &P6SEL0
		bic.b	#BIT0, &P6SEL1				; set as digital I/O port
		bis.b	#BIT0, &P6DIR				; set P6.0 as an output
		bis.b	#BIT0, &P6OUT				; init HIGH

		bic.b	#BIT1, &P6SEL0
		bic.b	#BIT1, &P6SEL1				; set as digital I/O port
		bis.b	#BIT1, &P6DIR				; set P6.1 as an output
		bis.b	#BIT1, &P6OUT				; init HIGH

		bic.b	#BIT2, &P6SEL0
		bic.b	#BIT2, &P6SEL1				; set as digital I/O port
		bis.b	#BIT2, &P6DIR				; set P6.2 as an output
		bis.b	#BIT2, &P6OUT				; init HIGH

		bic.b	#BIT3, &P6SEL0
		bic.b	#BIT3, &P6SEL1				; set as digital I/O port
		bis.b	#BIT3, &P6DIR				; set P6.3 as an output
		bis.b	#BIT3, &P6OUT				; init HIGH

		bic.b	#BIT4, &P6SEL0
		bic.b	#BIT4, &P6SEL1				; set as digital I/O port
		bis.b	#BIT4, &P6DIR				; set P6.4 as an output
		bis.b	#BIT4, &P6OUT				; init HIGH

		bic.b	#BIT7, &P3SEL0
		bic.b	#BIT7, &P3SEL1				; set as digital I/O port
		bis.b	#BIT7, &P3DIR				; set P3.7 as an output
		bis.b	#BIT7, &P3OUT				; init HIGH

		bic.b	#BIT4, &P2SEL0
		bic.b	#BIT4, &P2SEL1				; set as digital I/O port
		bis.b	#BIT4, &P2DIR				; set P2.4 as an output
		bis.b	#BIT4, &P2OUT				; init HIGH

		bic.b	#BIT3, &P3SEL0
		bic.b	#BIT3, &P3SEL1				; set as digital I/O port
		bis.b	#BIT3, &P3DIR				; set P3.3 as an output
		bis.b	#BIT3, &P3OUT				; init HIGH

		mov.w	#00000000b, R6				; Rx

;-- Setup Timer B1
		bis.w	#TBCLR, &TB1CTL
		bis.w	#TBSSEL__ACLK, &TB1CTL
		bis.w	#MC__UP, &TB1CTL


;-- Setup Compare Registers
		mov.w	#32849d, &TB1CCR0

		bis.w	#CCIE, &TB1CCTL0	; Setup TB1CCTL0 flags
		bic.w	#CCIFG, &TB1CCTL0


; Enable Global Interrupts
		bis.b	#GIE, SR


		bic.b	#LOCKLPM5, &PM5CTL0			;Turn on digital I/O


main:

		call 	#CheckKeypad
		call	#CheckPattern
		call	#OutputLED

		jmp		main						; loop main


;-------------------------------------------------------------------------------
; Long Delay
;-------------------------------------------------------------------------------
LongDelay:
		mov.w	#09h, R5					; Set Inner Delay Loop

LongFor:
		dec		R5								; decrement inner Delay loop
		jnz		LongFor							; loop through for loop
EndLongFor:

		dec		R4								; decrement outer Delay loop
		jnz		LongDelay						; jump to the beginning of Delay subroutine

		ret									; return
;--------------------------------- END Long Delay -----------------------------------
;-------------------------------------------------------------------------------
; Check Keypad
;-------------------------------------------------------------------------------
CheckKeypad:

		;D1 = P1.3
		bis.b	#BIT3, &P1REN				; Enable Resistors
		bic.b	#BIT3, &P1OUT				; use pull down resistor
		bic.b	#BIT3, &P1DIR				; set P1.3 as an input
		;D2 = P3.4
		bis.b	#BIT4, &P3REN				; Enable Resistors
		bic.b	#BIT4, &P3OUT				; use pull down resistor
		bic.b	#BIT4, &P3DIR				; set P3.4 as an input
		;D3 = P4.5
		bis.b	#BIT5, &P4REN				; Enable Resistors
		bic.b	#BIT5, &P4OUT				; use pull down resistor
		bic.b	#BIT5, &P4DIR				; set P4.5 as an input
		;D4 = P5.2
		bis.b	#BIT2, &P5REN				; Enable Resistors
		bic.b	#BIT2, &P5OUT				; use pull down resistor
		bic.b	#BIT2, &P5DIR				; set P5.2 as an input

		;D5 = P3.6
		bis.b	#BIT6, &P3DIR				; set P3.6 as an output
		bis.b	#BIT6, &P3OUT				; set HIGH
		;D6 = P1.7
		bis.b	#BIT7, &P1DIR				; set P1.7 as an output
		bis.b	#BIT7, &P1OUT				; set HIGH
		;D7 = P1.6
		bis.b	#BIT6, &P1DIR				; set P1.6 as an output
		bis.b	#BIT6, &P1OUT				; set HIGH
		;D8 = P1.5
		bis.b	#BIT5, &P1DIR				; set P1.5 as an output
		bis.b	#BIT5, &P1OUT				; set HIGH

		bit.b	#00001000b, &P1IN			; look at D1
		jnz		D1Set
		bit.b	#00010000b, &P3IN			; look at D2
		jnz		D2Set
		bit.b	#00100000b, &P4IN			; look at D3
		jnz		D3Set
		bit.b	#00000100b, &P5IN			; look at D4
		jnz		D4Set

		ret

D1Set:
		bic.b	#00001111b, Rx
		bis.b	#0001b, Rx
		jmp		CheckRows

D2Set:
		bic.b	#00001111b, Rx
		bis.b	#0010b, Rx
		jmp		CheckRows

D3Set:
		bic.b	#00001111b, Rx
		bis.b	#0100b, Rx
		jmp		CheckRows

D4Set:
		bic.b	#00001111b, Rx
		bis.b	#1000b, Rx
		jmp		CheckRows

CheckRows:

		;D5 = P3.6
		bis.b	#BIT6, &P3REN				; Enable Resistors
		bic.b	#BIT6, &P3OUT				; use pull down resistor
		bic.b	#BIT6, &P3DIR				; set P3.6 as an input
		;D6 = P1.7
		bis.b	#BIT7, &P1REN				; Enable Resistors
		bic.b	#BIT7, &P1OUT				; use pull down resistor
		bic.b	#BIT7, &P1DIR				; set P1.7 as an input
		;D7 = P1.6
		bis.b	#BIT6, &P1REN				; Enable Resistors
		bic.b	#BIT6, &P1OUT				; use pull down resistor
		bic.b	#BIT6, &P1DIR				; set P1.6 as an input
		;D8 = P1.5
		bis.b	#BIT5, &P1REN				; Enable Resistors
		bic.b	#BIT5, &P1OUT				; use pull down resistor
		bic.b	#BIT5, &P1DIR				; set P1.5 as an input

		;D1 = P1.3
		bis.b	#BIT3, &P1DIR				; set P1.3 as an output
		bis.b	#BIT3, &P1OUT				; set HIGH
		;D2 = P3.4
		bis.b	#BIT4, &P3DIR				; set P3.4 as an output
		bis.b	#BIT4, &P3OUT				; set HIGH
		;D3 = P4.5
		bis.b	#BIT5, &P4DIR				; set P4.5 as an output
		bis.b	#BIT5, &P4OUT				; set HIGH
		;D4 = P5.2
		bis.b	#BIT2, &P5DIR				; set P5.2 as an output
		bis.b	#BIT2, &P5OUT				; set HIGH

		bit.b	#01000000b, &P3IN			; look at D5
		jnz		D5Set
		bit.b	#10000000b, &P1IN			; look at D6
		jnz		D6Set
		bit.b	#01000000b, &P1IN			; look at D7
		jnz		D7Set
		bit.b	#00100000b, &P1IN			; look at D8
		jnz		D8Set

		ret
D5Set:
		bic.b	#11110000b, Rx
		bis.b	#00010000b, Rx
		ret

D6Set:
		bic.b	#11110000b, Rx
		bis.b	#00100000b, Rx
		ret

D7Set:
		bic.b	#11110000b, Rx
		bis.b	#01000000b, Rx
		ret

D8Set:
		bic.b	#11110000b, Rx
		bis.b	#10000000b, Rx
		ret


		ret									; return
;--------------------------------- END Check Keypad -----------------------------------
;-------------------------------------------------------------------------------
; CheckPattern
;-------------------------------------------------------------------------------

CheckPattern:
;if code = 81 => pattern 1
;if code = 41 => pattern 2
;if code = 21 => pattern 3
;if code = 11 => pattern 4

		cmp.b	#81h, Rx
		jz		SetPattern0
		cmp.b	#82h, Rx
		jz		SetPattern1
		cmp.b	#84h, Rx
		jz		SetPattern2
		cmp.b	#88h, Rx
		jz		SetPattern3


		ret

SetPattern0:
		mov.w	#00000000b, SetPattern
		mov.w	#10101010b, Output
		mov.w	#00h, Rx
		ret

SetPattern1:
		mov.w	#00000001b, SetPattern
		mov.w	#00000000b, Output
		mov.w	#00h, Rx
		ret

SetPattern2:
		mov.w	#00000010b, SetPattern
		mov.w	#01111111b, Output
		mov.w	#01b, R9
		mov.w	#00h, Rx
		ret

SetPattern3:
		mov.w	#00000011b, SetPattern
		mov.w	#00011000b, Output
		mov.w	#00h, Rx
		ret


;--------------------------------- END Check Keypad -----------------------------------
;-------------------------------------------------------------------------------
; Output
;-------------------------------------------------------------------------------

OutputLED:
		mov.w	Output, R8
		bit.b	#00000001b, R8
		jz		LED1off
		bis.b	#BIT0, &P6OUT				; set HIGH
		jmp		LED2
LED1off:
		bic.b	#BIT0, &P6OUT

LED2:
		mov.w	Output, R8
		bit.b	#00000010b, R8
		jz		LED2off
		bis.b	#BIT1, &P6OUT				; set HIGH
		jmp		LED3
LED2off:
		bic.b	#BIT1, &P6OUT

LED3:
		mov.w	Output, R8
		bit.b	#00000100b, R8
		jz		LED3off
		bis.b	#BIT2, &P6OUT				; set HIGH
		jmp		LED4
LED3off:
		bic.b	#BIT2, &P6OUT

LED4:
		mov.w	Output, R8
		bit.b	#00001000b, R8
		jz		LED4off
		bis.b	#BIT3, &P6OUT				; set HIGH
		jmp		LED5
LED4off:
		bic.b	#BIT3, &P6OUT

LED5:
		mov.w	Output, R8
		bit.b	#00010000b, R8
		jz		LED5off
		bis.b	#BIT4, &P6OUT				; set HIGH
		jmp		LED6
LED5off:
		bic.b	#BIT4, &P6OUT

LED6:
		mov.w	Output, R8
		bit.b	#00100000b, R8
		jz		LED6off
		bis.b	#BIT7, &P3OUT				; set HIGH
		jmp		LED7
LED6off:
		bic.b	#BIT7, &P3OUT

LED7:
		mov.w	Output, R8
		bit.b	#01000000b, R8
		jz		LED7off
		bis.b	#BIT4, &P2OUT				; set HIGH
		jmp		LED8
LED7off:
		bic.b	#BIT4, &P2OUT

LED8:
		mov.w	Output, R8
		bit.b	#10000000b, R8
		jz		LED8off
		bis.b	#BIT3, &P3OUT				; set HIGH
		jmp		LEDend
LED8off:
		bic.b	#BIT3, &P3OUT

LEDend:
		ret
;--------------------------------- END Output -----------------------------------
;-------------------------------------------------------------------------------
; Interrupt Service Routines
;-------------------------------------------------------------------------------
TimerB1_Switch:
		cmp.b		#00h, SetPattern
		jz			Pattern0

		cmp.b		#01h, SetPattern
		jz			Pattern1

		cmp.b		#02h, SetPattern
		jz			Pattern2

		cmp.b		#03h, SetPattern
		jz			Pattern3

Pattern0:
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti
Pattern1:
		inc.b	Output
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti
Pattern2:
		inc.b	R7
		bit.b	#01b, R7
		jz		Pattern2a
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti
Pattern2a:
		bit.b	#01b, R9			; check carry flag
		jz		Pattern2b
		setc

Pattern2b:
		bic.b	#01b, R9
		rrc.b	Output
		adc.b	R9
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti
Pattern3:
		inc.b	R7
		bit.b	#01b, R7
		jz		Pattern3Start
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti

Pattern3Start:
		inc.b	R10
		cmp.b	#01h, R10
		jz		Pattern3a
		cmp.b	#02h, R10
		jz		Pattern3b
		cmp.b	#03h, R10
		jz		Pattern3c
		cmp.b	#04h, R10
		jz		Pattern3d
		cmp.b	#05h, R10
		jz		Pattern3e
		cmp.b	#06h, R10
		jz		Pattern3f

Pattern3a:
		mov.w	#00011000b, Output
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti

Pattern3b:
		mov.w	#00100100b, Output
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti

Pattern3c:
		mov.w	#01000010b, Output
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti

Pattern3d:
		mov.w	#10000001b, Output
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti

Pattern3e:
		mov.w	#01000010b, Output
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti

Pattern3f:
		mov.w	#00100100b, Output
		mov.w	#00h, R10
		bic.w	#CCIFG, &TB1CCTL0	; clear flag
		reti


;--------- END TB1 ISRs --------------------------------------------------------

;-------------------------------------------------------------------------------
; Memory Allocation
;-------------------------------------------------------------------------------

			.data								; go to data memory
			.retain								; keep this section

Rx:			.space	2

SetPattern:	.space 	2

Output:		.space	2


;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
            .sect	".int41"				; TB1CCR0 Vector
            .short	TimerB1_Switch
