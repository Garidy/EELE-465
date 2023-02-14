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

		mov.w	#00000000b, R6				; Rx


		bic.b	#LOCKLPM5, &PM5CTL0			;Turn on digital I/O


main:

		call 	#CheckKeypad

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
		bis.b	#0001b, R6
		jmp		CheckRows

D2Set:
		bis.b	#0010b, R6
		jmp		CheckRows

D3Set:
		bis.b	#0100b, R6
		jmp		CheckRows

D4Set:
		bis.b	#1000b, R6
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
		bis.b	#00010000b, R6
		ret

D6Set:
		bis.b	#00100000b, R6
		ret

D7Set:
		bis.b	#01000000b, R6
		ret

D8Set:
		bis.b	#10000000b, R6
		ret


		ret									; return
;--------------------------------- END Check Keypad -----------------------------------
;-------------------------------------------------------------------------------
; Memory Allocation
;-------------------------------------------------------------------------------

		.data								; go to data memory
		.retain								; keep this section

Rx:	.space	2


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
            
