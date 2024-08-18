;/*
; interrupt handlers
;*/

;  SOME CPU RELATED DEFINITIONS
;
Mode_Mask		EQU	0x1F           ; Mask for CPU mode bits
Mode_USR        	EQU     0x10
Mode_FIQ		EQU	0x11
Mode_IRQ        	EQU     0x12
Mode_SVC        	EQU     0x13
Mode_ABT		EQU     0x17
Mode_UND		EQU	0x1B
Mode_SYS		EQU	0x1F

T_Bit			EQU	0x20           ; THUMB mode bit
I_Bit           	EQU     0x80
F_Bit           	EQU     0x40
Int_Bits		EQU	(I_Bit+F_Bit)  ; IRQ+FIQ disable bits

; special SWI for CMX
SWI_SVC_Mode    EQU  0xFF                      ; SWI to switch to SVC mode

; The following are for the LPC2368
VIC_BASE        EQU  0xFFFFF000
VIC_ADDR        EQU  0xF00

; --------------------------------------------------------------------------

 IF :DEF:CS_RTOS
	IMPORT	int_count
	IMPORT	K_OS_Intrp_Exit
 ENDIF

        EXPORT Undef_Wrapper
        EXPORT SWI_Wrapper
        EXPORT PAbt_Wrapper
        EXPORT DAbt_Wrapper
        EXPORT IRQ_Wrapper
        EXPORT FIQ_Wrapper

        PRESERVE8
        AREA |int_handlers|, CODE, READONLY
        ARM

; The exception handlers use C functions found in cmx_isrs.c. It is up
; to the user to add code to those functions. Except for the SWI handler
; The handlers use the int_count variable and the CMX interrupt exit so
; CMX interrupt-safe routines can be called from the C Handlers.

;
;	Undefined Instruction  (regs=und)
;
	IMPORT	cmx_undef
Undef_Wrapper
   STMFD SP!,{R0-R12,LR}               ; save the registers
   MRS   R9,SPSR                       ; save the SPSR
   STMFD SP!,{R9}
 IF :DEF:CS_RTOS
   LDR   R2,=int_count                 ; get int_count
   MRS   R3,CPSR
   MOV   R4,R3
   ORR	 R4,R4,#Int_Bits               ;  disable interrupts
   MSR	 CPSR_c,R4
   LDRB	 R1,[R2]                    ;  incr the count
   ADD 	 R1,R1,#1
   STRB	 R1,[R2]
   MSR   CPSR_c,R3                  ;  restore interrupts
   ldr   r0,=cmx_undef              ; call C handler
   LDR   LR,=K_OS_Intrp_Exit        ; set up the C handler to
   BX    R0                         ; return directly to CMX interrupt exit
 ELSE
   ldr   r0,=cmx_undef
   mov   lr,pc
   BX    R0
   LDMFD SP!,{R4}
   MSR   SPSR_cxsf,R4                ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^            ; restore regs and return
 ENDIF

;
;	Prefetch Exception
;
	IMPORT	cmx_prefetch
PAbt_Wrapper
   SUB   LR,LR,#4                      ; adj for proper CMX return
   STMFD SP!,{R0-R12,LR}               ; save the registers
   MRS   R9,SPSR                       ; save the SPSR
   STMFD SP!,{R9}
 IF :DEF:CS_RTOS
   LDR   R2,=int_count                 ; get int_count
   MRS   R3,CPSR
   MOV   R4,R3
   ORR	 R4,R4,#Int_Bits               ;  disable interrupts
   MSR	 CPSR_c,R4
   LDRB	 R1,[R2]                    ;  incr the count
   ADD 	 R1,R1,#1
   STRB	 R1,[R2]
   MSR   CPSR_c,R3                  ;  restore interrupts
   ldr   r0,=cmx_prefetch           ; call C handler
   LDR   LR,=K_OS_Intrp_Exit        ; set up the C handler to
   BX    R0                         ; return directly to CMX interrupt exit
 ELSE
   ldr   r0,=cmx_prefetch
   mov   lr,pc
   BX    R0
   LDMFD SP!,{R4}
   MSR   SPSR_cxsf,R4                ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^            ; restore regs and return
 ENDIF

;
;	Data Abort Exception
;
	IMPORT	cmx_abort
DAbt_Wrapper
   SUB   LR,LR,#8                      ; adj for proper CMX return
   STMFD SP!,{R0-R12,LR}               ; save the registers
   MRS   R9,SPSR                       ; save the SPSR
   STMFD SP!,{R9}
 IF :DEF:CS_RTOS
   LDR   R2,=int_count                 ; get int_count
   MRS   R3,CPSR
   MOV   R4,R3
   ORR	 R4,R4,#Int_Bits               ;  disable interrupts
   MSR	 CPSR_c,R4
   LDRB	 R1,[R2]                    ;  incr the count
   ADD 	 R1,R1,#1
   STRB	 R1,[R2]
   MSR   CPSR_c,R3                  ;  restore interrupts
   ldr   r0,=cmx_abort              ; call C handler
   LDR   LR,=K_OS_Intrp_Exit        ; set up the C handler to
   BX    R0                         ; return directly to CMX interrupt exit
 ELSE
   ldr   r0,=cmx_abort
   mov   lr,pc
   BX    R0
   LDMFD SP!,{R4}
   MSR   SPSR_cxsf,R4                ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^            ; restore regs and return
 ENDIF

;
;	Software Interrupt
;
;  The handler calls the C Handler in cmx_isrs.c with the parameter being
;  the SWI number. The SWI will ultimately return in the same processor mode
;  that it was originally called in. 
;
	IMPORT	cmx_swi
SWI_Wrapper
   STMFD SP!,{R0-R12,LR}               ; save regs
   MRS   R4,SPSR                       ; save SPSR
   STMFD SP!,{R4}
   TST   R4,#T_Bit                     ; SWI from Thumb mode?
   LDREQ R0,[LR,#-4]                   ; get ARM SWI #
   BICEQ R0,R0,#0xFF000000             ; mask off the instruction part
   LDRNEH R0,[LR,#-2]                  ; get Thumb SWI #
   BICNE  R0,R0,#0x0000FF00            ; mask off the instruction part
   CMP   R0,#SWI_SVC_Mode              ; is this SVC mode call?
   BNE   %1                            ; if not, skip forward
   BIC   R4,R4,#Mode_Mask              ; else, set to SPSR to SVC mode
   ORR   R4,R4,#Mode_SVC
   STR   R4,[SP]
   B     %2                            ; goto restore and return
1
   ldr  r5,=cmx_swi
   mov  lr,pc
   bx   r5

2
   LDMFD SP!,{R4}                      ; restore and return
   MSR   SPSR_cxsf,R4                  ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^              ; restore regs and return

;
;	IRQ Interrupt
;

IRQ_Wrapper
   SUB   LR,LR,#4                      ; adj for proper CMX return
   STMFD SP!,{R0-R12,LR}               ; save the registers
   MRS   R9,SPSR                       ; get the SPSR
   STMFD SP!,{R9}                      ; save the SPSR

 IF :DEF:CS_RTOS
   LDR   R2,=int_count                 ; get int_count
   MRS   R3,CPSR
   MOV   R4,R3
   ORR	 R4,R4,#Int_Bits               ; disable interrupts
   MSR	 CPSR_c,R4
   LDRB	 R1,[R2]                       ; incr the count
   ADD 	 R1,R1,#1
   STRB	 R1,[R2]
   MSR   CPSR_c,R3                     ; restore interrupts
 ENDIF

   ldr   r12, =VIC_BASE                ; get the address of the handler
   ldr   r0, [r12, #VIC_ADDR]
   cmp   r0, #0                        ; handler available?
   beq   irq_ack                       ; no, ack interrupt and exit

   LDR   LR,=irq_ack
   bx    r0

irq_ack
; ACK the interrupt
   ldr   r12, =VIC_BASE
   str   r0, [r12, #VIC_ADDR]

 IF :DEF:CS_RTOS
   ldr   pc,=K_OS_Intrp_Exit
 ELSE
   LDMFD SP!,{R4}
   MSR   SPSR_cxsf,R4                ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^            ; restore regs and return
 ENDIF

;
;	Fast FIQ Interrupt
;
	IMPORT	cmx_fiq
FIQ_Wrapper
   SUB   LR,LR,#4                      ; adj for proper CMX return
   STMFD SP!,{R0-R12,LR}               ; save the registers
   MRS   R9,SPSR                       ; get the SPSR
   STMFD SP!,{R9}                      ; save the SPSR
 IF :DEF:CS_RTOS
   LDR   R2,=int_count                 ; get int_count
   LDRB	 R1,[R2]                       ; incr the count
   ADD 	 R1,R1,#1
   STRB	 R1,[R2]
   ldr   r0,=cmx_fiq                   ; call C handler
   mov   lr,pc
   BX    R0

   MRS   R0,SPSR                       ; get the SPSR
   BIC   r0,r0,#0xffffffe0	       ; clear all excepts bits 0-4
   mov   r1,#Mode_IRQ
   cmp   r0,r1                         ; Did we interrupt the IRQ handler?
   bne   FIQ_Wrapper_exit              ; No, go to K_OS_Intrp_Exit
   LDR   R2,=int_count                 ; get int_count
   LDRB	 r1,[r2,#0]
   mov   r0,#1
   cmp   r0,r1                         ; int_count equals 1?
   bne   FIQ_Wrapper_exit              ; No, go to K_OS_Intrp_Exit

   ; If we get here the IRQ handler was interrupted before it could
   ; increment the int_count. So just return to the IRQ handler.
   LDMFD SP!,{R4}
   MSR   SPSR_cxsf,R4                ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^            ; restore regs and return

FIQ_Wrapper_exit
   ldr   pc,=K_OS_Intrp_Exit
 ELSE
   ldr   r0,=cmx_fiq
   mov   lr,pc
   BX    R0
   LDMFD SP!,{R4}
   MSR   SPSR_cxsf,R4                ; restore SPSR
   LDMFD SP!,{R0-R12,PC}^            ; restore regs and return
 ENDIF

	END
