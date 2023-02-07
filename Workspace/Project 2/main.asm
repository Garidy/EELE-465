;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
; J.Wurst and G.Sanders, EELE465, Project 02
; Jan 26, 2023
; Read and Write to an RTC using I2C.
;
;	Registers
;		R4: outer delay loop counter (set before call to delay)
;		R5: inner delay loop counter (set inside "Delay" subroutine)
;		R6: TransmitByte counter
;
;	Variables
;		TransmitByte: Data being transmited out on SDA
;		DataOut: Data to be shifted into TransmitByte
;
;	Sub Routines
;		Short Delay: Delay for a few clock cycles
;		Long Delay:	Delay for next 15kHz clock
;		I2Cstart: Send I2C start condition (SDA pulled low, short delay, SCL pulled low)
;		I2Csend: Load Transmit Byte with data to be sent and acknowledge request
;		I2CtxByte: Load Transmit Byte and shift to MSB, send out bit by bit
;		I2CackRequest: Put SDA into input and wait for acknowledge bit from slave
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

		bic.b	#BIT0, &P3SEL0
		bic.b	#BIT0, &P3SEL1				; set as digital I/O port
		bis.b	#BIT0, &P3DIR				;Set P3.0 as an output (SDA)
		bis.b	#BIT0, &P3OUT				;Initialize SDA HIGH

		bic.b	#LOCKLPM5, &PM5CTL0			;Turn on digital I/O


main:
		call 	#I2Cstart
		mov.w	#01h, R4

		mov.b	#011010000b, TransmitByte	;Set TransmitByte to slave address (1101000) and R/W bit (0:W 1:R)

		call	#I2Csend

		;mov.w	#0FFFFh, R4					; Set Outer Delay Loop

		;call	#Delay						; call Delay subroutine
		bis.b	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #01h, R4
		call	#LongDelay
		bis.b	#BIT0, &P3OUT				; SDA pulled to a logic high

		jmp		main						; loop main
;-------------------------------------------------------------------------------
; Short Delay
;-------------------------------------------------------------------------------
ShortDelay:
		mov.w	#04h, R5					; Set Inner Delay Loop

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

		mov.w   #01h, R4
		call	#ShortDelay

		bic.b	#BIT2, &P3OUT				; SCL pulled to a logic low
		ret									;return to main
;--------------------------------- END I2Cstart -----------------------------------

;-------------------------------------------------------------------------------
; I2Csend
;-------------------------------------------------------------------------------
I2Csend:
		mov.w	#08h, R6					; sets TxCounter to 8
		call	#I2CtxByte					; calls I2CtxByte
		call 	#I2CackRequest				; calls I2CackRequest

		mov.w	#08h, R6					; sets TxCounter to 8
		mov.w	#00h, TransmitByte			; write to RTC seconds register
		call	#I2CtxByte					; calls I2CtxByte
		call 	#I2CackRequest				; calls I2CackRequest

;		bis.b	#BIT2, &P3OUT				; SCL pulled to a logic high
;		mov.w   #02h, R4
;		call	#ShortDelay
;		bis.b	#BIT0, &P3OUT				; SDA pulled to a logic high
		call 	#I2Cstart

		mov.b	#011010001b, TransmitByte	;Set TransmitByte to slave address (1101000) and R/W bit (0:W 1:R)
		mov.w	#08h, R6					; sets TxCounter to 8
		call	#I2CtxByte					; calls I2CtxByte
		call 	#I2CackRequest				; calls I2CackRequest

		mov.w	#08h, R6					; sets RxCounter to 8
		call	#I2CrxByte					; calls I2CrxByte
		call	#I2CsendAck
		mov.w	#08h, R6					; sets RxCounter to 8
		call	#I2CrxByte					; calls I2CrxByte
		call	#I2CsendAck
		mov.w	#08h, R6					; sets RxCounter to 8
		call	#I2CrxByte					; calls I2CrxByte
		call	#I2CsendNak

		bis.b	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #01h, R4
		call	#LongDelay
		bis.b	#BIT0, &P3OUT				; SDA pulled to a logic high


		call 	#I2Cstart
		mov.w	#01h, R4


		mov.b	#011010000b, TransmitByte	;Set TransmitByte to slave address (1101000) and R/W bit (0:W 1:R)
		mov.w	#08h, R6					; sets TxCounter to 8
		call	#I2CtxByte					; calls I2CtxByte
		call 	#I2CackRequest				; calls I2CackRequest

		mov.w	#08h, R6					; sets TxCounter to 8
		mov.w	#11h, TransmitByte			; write to RTC temp register
		call	#I2CtxByte					; calls I2CtxByte
		call 	#I2CackRequest				; calls I2CackRequest

		call 	#I2Cstart

		mov.b	#011010001b, TransmitByte	;Set TransmitByte to slave address (1101000) and R/W bit (0:W 1:R)
		mov.w	#08h, R6					; sets TxCounter to 8
		call	#I2CtxByte					; calls I2CtxByte
		call 	#I2CackRequest				; calls I2CackRequest

		mov.w	#08h, R6					; sets RxCounter to 8
		call	#I2CrxByte					; calls I2CrxByte
		call	#I2CsendAck
		mov.w	#08h, R6					; sets RxCounter to 8
		call	#I2CrxByte					; calls I2CrxByte
		call	#I2CsendNak

		ret									; return to main

;		mov.b 	#00h, DataOut				; initialize 00h into DataOut

;DataCount:
;		mov.b 	DataOut, TransmitByte
;		mov.w	#08h, R6
;		call	#I2CtxByte					; calls I2CtxByte
;		call 	#I2CackRequest				; calls I2CackRequest
;
;		inc.b 	DataOut						; increment DataOut
;		cmp.b	#00001010b, DataOut
;		jnz		DataCount					; if DataOut != 10, loop counter
;EndDataCount:
;		ret									;return to main

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

		mov.w   #01h, R4
;		call	#ShortDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #01h, R4
		call	#ShortDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low

		rla.b	TransmitByte	; set next bit to MSB
		dec.w	R6				; decrement TransmitByte Counter
		cmp.w	#00h, R6		; check if R6 is 0
		jnz		I2CtxByte		; loop until R6 = 0

		ret									;return to I2Csend
;--------------------------------- END I2CtxByte -----------------------------------
;-------------------------------------------------------------------------------
; I2CrxByte
;-------------------------------------------------------------------------------
I2CrxByte:

		bic.b	#BIT0, &P3DIR				; Set P3.0 as an input
		bis.b	#BIT0, &P3REN				; Enable Resistors
		bis.b	#BIT0, &P3OUT				; use pull up resistor

		mov.w   #01h, R4
;		call	#ShortDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high

		bit.b	#00000001, &P3IN			; look at P3.0
		jz		ReadByte1

ReadByte0:
		bis.b	#00000000b, ReadByte
		jmp		ReadByteEnd

ReadByte1:
		bis.b	#00000001b, ReadByte
		jmp		ReadByteEnd

ReadByteEnd:


		mov.w   #01h, R4
;		call	#ShortDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low

		rla.b	ReadByte					; set next bit to MSB
		dec.w	R6							; decrement TransmitByte Counter
		cmp.w	#00h, R6					; check if R6 is 0
		jnz		I2CrxByte					; loop until R6 = 0

		ret									;return to I2Csend

;--------------------------------- END I2CrxByte -----------------------------------

;-------------------------------------------------------------------------------
; I2CsendAck
;-------------------------------------------------------------------------------
I2CsendAck:

		bis.b	#BIT0, &P3DIR				; Set P3.0 as an output (SDA)
;		bic.b	#BIT0, &P3REN				; Disable Resistors
		; send ack
		bic.b	#BIT0, &P3OUT				; SDA pulled to a logic low
		mov.w   #01h, R4
		call	#ShortDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #01h, R4
		call	#LongDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low



		ret									;return to I2Csend
;--------------------------------- END I2CsendAck -----------------------------------
;-------------------------------------------------------------------------------
; I2CsendNak
;-------------------------------------------------------------------------------
I2CsendNak:

		bis.b	#BIT0, &P3DIR				; Set P3.0 as an output (SDA)
;		bic.b	#BIT0, &P3REN				; Disable Resistors
		; send ack
		bis.b	#BIT0, &P3OUT				; SDA pulled to a logic high
		mov.w   #01h, R4
		call	#ShortDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #01h, R4
		call	#LongDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low



		ret									;return to I2Csend
;--------------------------------- END I2CsendAck -----------------------------------
;-------------------------------------------------------------------------------
; I2CackRequest
;-------------------------------------------------------------------------------
I2CackRequest:

;		bic.b	#BIT0, &P3OUT				; SDA pulled to a logic low !!!DELETE THIS LATER!!!

		bic.b	#BIT0, &P3DIR				; Set P3.0 as an input
		bis.b	#BIT0, &P3REN				; Enable Resistors
		bis.b	#BIT0, &P3OUT				; use pull up resistor

		mov.w   #01h, R4
		call	#LongDelay
		bis.b 	#BIT2, &P3OUT				; SCL pulled to a logic high
		mov.w   #01h, R4
		call	#ShortDelay
		bic.b 	#BIT2, &P3OUT				; SCL pulled to a logic low

;		bit.b	#00000001b, &P3IN			; look at P3.0
;		mov.b	&P3IN, R7

;		jz		I2CackRequest

;		mov.w   #02h, R4
;		call	#ShortDelay

		bis.b	#BIT0, &P3DIR				; Set P3.0 as an output (SDA)
		bic.b	#BIT0, &P3REN				; Disable Resistors

		ret									;return to I2Csend
;--------------------------------- END I2CackRequest -----------------------------------


;-------------------------------------------------------------------------------
; Memory Allocation
;-------------------------------------------------------------------------------

		.data								; go to data memory
		.retain								; keep this section

SlaveAddress:	.short	001101000b

TransmitByte:	.space	2

DataOut:		.space  2

ReadByte:		.space  2


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
            
