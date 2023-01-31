;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
; J.Wurst and G.Sanders, EELE465, Project 02
; Jan 26, 2023
; Read and Write to an RTC using I2C.
;
;		R4: outer delay loop counter (set before call to delay)
;		R5: inner delay loop counter (set inside "Delay" subroutine)
;		R6: TransmitByte counter
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
		bis.b	#BIT2, &P3OUT				;Initialize SCL HIGH

		bis.b	#BIT0, &P3DIR				;Set P3.3 as an output (SDA)
		bis.b	#BIT0, &P3OUT				;Initialize SDA HIGH

		bic.b	#LOCKLPM5, &PM5CTL0			;Turn on digital I/O


main:
		call 	#I2Cstart
		mov.w	#08h, R6
		mov.w	#02h, R4

		mov.b	#011010000b, TransmitByte	;Set TransmitByte to slave address (1101000) and R/W bit (0:W 1:R)

		call	#I2Csend

		;mov.w	#0FFFFh, R4					; Set Outer Delay Loop

		;call	#Delay						; call Delay subroutine
		bis.b	#BIT2, &P3OUT				; SCL pulled to a logic high
		bis.b	#BIT0, &P3OUT				; SDA pulled to a logic high

		jmp		main						; loop main
;-------------------------------------------------------------------------------
; Short Delay
;-------------------------------------------------------------------------------
ShortDelay:
		mov.w	#01h, R5					; Set Inner Delay Loop

ShortFor:
		dec		R5							; decrement inner Delay loop
		jnz		ShortFor							; loop through for loop
EndShortFor:

		dec		R4							; decrement outer Delay loop
		jnz		ShortDelay						; jump to the beginning of Delay subroutine

		ret									; return
;--------------------------------- END Short Delay -----------------------------------

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
;--------------------------------- END Short Delay -----------------------------------

;-------------------------------------------------------------------------------
; I2Cstart
;-------------------------------------------------------------------------------
I2Cstart:
;		bis.b	#BIT0, &P3OUT				; Force SDA HIGH
		bis.b	#BIT2, &P3OUT				; Force SCL HIGH

		bic.b	#BIT0, &P3OUT				; SDA pulled to a logic low

		mov.w   #02h, R4
		call	#ShortDelay

		bic.b	#BIT2, &P3OUT				; SCL pulled to a logic low
		ret									;return to main
;--------------------------------- END I2Cstart -----------------------------------

;-------------------------------------------------------------------------------
; I2Csend
;-------------------------------------------------------------------------------
I2Csend:

		call	#I2CtxByte					; calls I2CtxByte

		call 	#I2CackRequest				; calls I2CackRequest

		ret									;return to main
;--------------------------------- END I2Csend -----------------------------------

;-------------------------------------------------------------------------------
; I2CtxByte
;-------------------------------------------------------------------------------
I2CtxByte:

		bit.w 	#010000000b, TransmitByte	;test if MSB of TransmitByte = 1
		jnz		TransmitByte1

TransmitByte0:
		bic.b	#BIT0, &P3OUT				; SDA pulled to a logic low
		jmp		TransmitByteEnd

TransmitByte1:
		bis.b	#BIT0, &P3OUT				; SDA pulled to a logic high
		jmp		TransmitByteEnd

TransmitByteEnd:

		mov.w   #02h, R4
		call	#ShortDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #02h, R4
		call	#LongDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low

		rla.b	TransmitByte	; set next bit to MSB
		dec.w	R6				; decrement TransmitByte Counter
		cmp.w	#00h, R6		; check if R6 is 0
		jnz		I2CtxByte		; loop until R6 = 0

		ret									;return to I2Csend
;--------------------------------- END I2CtxByte -----------------------------------

;-------------------------------------------------------------------------------
; I2CackRequest
;-------------------------------------------------------------------------------
I2CackRequest:

		bic.b	#BIT0, &P3OUT				; SDA pulled to a logic low !!!DELETE THIS LATER!!!

		mov.w   #02h, R4
		call	#ShortDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #02h, R4
		call	#LongDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low

		ret									;return to I2Csend
;--------------------------------- END I2Csend -----------------------------------


;-------------------------------------------------------------------------------
; Memory Allocation
;-------------------------------------------------------------------------------

		.data								; go to data memory
		.retain								; keep this section

SlaveAddress:	.short	001101000b

TransmitByte:	.space	2


;-------------------------------------------------------------------------------


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
            
