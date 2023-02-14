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


		bic.b	#LOCKLPM5, &PM5CTL0			;Turn on digital I/O


main:


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
            
