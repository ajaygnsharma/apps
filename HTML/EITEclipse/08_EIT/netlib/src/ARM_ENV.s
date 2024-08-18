; Interrupt enable and disable functions for CMX-MicroNet in polled mode.
; ARM Realview tools.

I_Bit		EQU	0x80
F_Bit		EQU	0x40
Int_Bits	EQU	0xC0

FIQ_ON          EQU     0	;set to 1 to allow for fiq interrupts

 [ FIQ_ON = 1
INTS_ENABLE     EQU     Int_Bits
 |
INTS_ENABLE     EQU     I_Bit
 ]

	EXPORT	K_OS_Disable_Interrupts
	EXPORT	K_OS_Enable_Interrupts

;   PRESERVE8
   AREA |cmxenv|, CODE, READONLY
   CODE32

|x$codeseg| 

;
;  void K_OS_Disable_Interrupts (void)
;
;  This routine disables all interrupts.  
;
K_OS_Disable_Interrupts
	MRS	r0, cpsr
	ORR	r0,r0,#Int_Bits		; turn interrupts off
	MSR	cpsr_c,r0
;	MOV	pc,lr
        BX      lr

;
;  void K_OS_Enable_Interrupts (void)
;
;  This routine enables all interrupts. The definition of "all" is dependent
;  on the conditional assembly in terms of whether FIQ is used or not. These
;  bits are defined in INTS_ENABLE. 
;
K_OS_Enable_Interrupts
	MRS	r0, cpsr
	BIC	r0,r0,#INTS_ENABLE	; enable irq interrupts
	MSR	cpsr_c,r0
;	MOV	pc,lr
        BX      lr

	END

