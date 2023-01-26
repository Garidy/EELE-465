;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
; J.Wurst and G.Sanders, EELE465, Project 02
; Jan 26, 2023
; Read and Write to an RTC using I2C.
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

;PortX.SCL : P3.2
;PortX.SDA : P3.0
;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
init:
		bis.b	#BIT2, &P3DIR				;Set P3.2 as an output (SCL)
		bic.b	#BIT2, &P3OUT				;Initialize SCL low

		bis.b	#BIT0, &P3DIR				;Set P3.3 as an output (SDA)
		bic.b	#BIT0, &P3DIR				;Initialize SDA low

		bic.b	#LOCKLPM5, &PM5CTL0			;Turn on digital I/O


main:
		bis.b	#BIT0, &P3DIR				; Toggle P3.0 (LED)
		mov.w	#0FFFFh, R4					; Set Outer Delay Loop

		call	#Delay						; call Delay subroutine

		bic.b	#BIT0, &P3DIR				; Toggle P3.0 (LED)

		mov.w	#0FFFFh, R4					; Set Outer Delay Loop

		call	#Delay						; call Delay subroutine
;-------------------------------------------------------------------------------
; Delay start
;-------------------------------------------------------------------------------
Delay:
		mov.w	#04h, R5		; Set Inner Delay Loop

For:
		dec		R5				; decrement inner Delay loop
		jnz		For				; loop through for loop
EndFor:

		dec		R4				; decrement outer Delay loop
		jnz		Delay			; jump to the beginning of Delay subroutine

		ret						; return to main
;--------------------------------- END Delay -----------------------------------



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
            
