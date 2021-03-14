;------------------------------------------
; DSK_vectors_AIC23.asm (INT5 only on McBSP0)
; US 5-Feb-07; KUP 11.08
;-------------------------------------------
   	.title "C6x Interrupt Vectors "
	.sect	".vectors"			; Declare a section name which is used in the link-CMD file
	.ref	_c_int00			; program reset address
	.ref 	_intser_McBSP1	; User-ISR declaration
SP	.set	B15				; rts6700.lib uses B15 as stack pointer

    .def RESET_V
RESET_V:			; Reset
	MVKL 	_c_int00, B3
	MVKH	_c_int00, B3
	B		B3
	MVC 	PCE1, B0			; get address of interrupt vectors
	MVC 	B0, ISTP			; set table to point here
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def NMI_V
NMI_V:				; Non-maskable interrupt Vector
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def AINT_V
AINT_V:				; Analysis Interrupt Vector (reserved)
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT3_V
INT3_V:				; Maskable Interrupt #6
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	.align	32
	.def INT4_V

INT4_V:
	B		$
	NOP 	
	NOP 	
	NOP
	NOP
	NOP
	NOP             
 	NOP	 	
			
	.align	32
	.def INT5_V
INT5_V:								; Maskable Interrupt #5
	STW		B0,*SP--[2]				; save B0 temporarily
||	MVKL 		_intser_McBSP1, B0	; load ISR address
	MVKH		_intser_McBSP1, B0
	B		B0						; execute ISR
	LDW		*++SP[2],B0				; restore B0
	NOP		2
	NOP             
 	NOP	 	
	
	.align	32
	.def INT6_V
INT6_V:				; Maskable Interrupt #6
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT7_V
INT7_V:				; Maskable Interrupt #7
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT8_V
INT8_V:				; Maskable Interrupt #8
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT9_V
INT9_V:				; Maskable Interrupt #9
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT10_V
INT10_V: 			; Maskable Interrupt #10
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT11_V
INT11_V: 			; Maskable Interrupt #11
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT12_V
INT12_V: 			; Maskable Interrupt #12
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT13_V
INT13_V: 			; Maskable Interrupt #13
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT14_V
INT14_V: 			; Maskable Interrupt #14
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT15_V
INT15_V: 			; Maskable Interrupt #15
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
; the remainder of the vector table is reserved
	.end

