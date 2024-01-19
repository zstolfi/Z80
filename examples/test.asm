.org $8000
.dw $

label:
	DJNZ label  	;          
	JP   label  	;          
	JP   (HL)   	;          
	JP   (IX)   	;          
	JP   (IY)   	;          
	JP   C      	,	label  
	JP   M      	,	label  
	JP   NC     	,	label  
	JP   NZ     	,	label  
	JP   P      	,	label  
	JP   PE     	,	label  
	JP   PO     	,	label  
	JP   Z      	,	label  
	JR   label  	;          
	JR   NZ     	,	label  
	JR   Z      	,	label  
	JR   C      	,	label  
	JR   NC     	,	label  

	ADC  A      	,	(HL)   
	ADC  A      	,	(IX+00)
	ADC  A      	,	(IY+01)
	ADC  A      	,	A      
	ADC  A      	,	B      
	ADC  A      	,	C      
	ADC  A      	,	D      
	ADC  A      	,	E      
	ADC  A      	,	H      
	ADC  A      	,	L      
	ADC  A      	,	0      
	ADC  HL     	,	BC     
	ADC  HL     	,	DE     
	ADC  HL     	,	HL     
	ADC  HL     	,	SP     
	ADD  A      	,	(HL)   
	ADD  A      	,	(IX+02)
	ADD  A      	,	(IY+03)
	ADD  A      	,	A      
	ADD  A      	,	B      
	ADD  A      	,	C      
	ADD  A      	,	D      
	ADD  A      	,	E      
	ADD  A      	,	H      
	ADD  A      	,	L      
	ADD  A      	,	1      
	ADD  HL     	,	BC     
	ADD  HL     	,	DE     
	ADD  HL     	,	HL     
	ADD  HL     	,	SP     
	ADD  IX     	,	BC     
	ADD  IX     	,	DE     
	ADD  IX     	,	IX     
	ADD  IX     	,	SP     
	ADD  IY     	,	BC     
	ADD  IY     	,	DE     
	ADD  IY     	,	IY     
	ADD  IY     	,	SP     
	AND  (HL)   	;          
	AND  (IX+04)	;          
	AND  (IY+05)	;          
	AND  A      	;          
	AND  B      	;          
	AND  C      	;          
	AND  D      	;          
	AND  E      	;          
	AND  H      	;          
	AND  L      	;          
	AND  2      	;          
	BIT  0      	,	(HL)   
	BIT  1      	,	(IX+06)
	BIT  2      	,	(IY+07)
	BIT  3      	,	A      
	BIT  4      	,	B      
	BIT  5      	,	C      
	BIT  6      	,	D      
	BIT  7      	,	E      
	BIT  0      	,	H      
	BIT  1      	,	L      
	CALL C      	,	1000   
	CALL M      	,	1001   
	CALL NC     	,	1002   
	CALL NC     	,	1003   
	CALL 1004   	;          
	CALL NZ     	,	1005   
	CALL P      	,	1006   
	CALL PE     	,	1007   
	CALL PO     	,	1008   
	CALL Z      	,	1009   
	CCF  ;      	;          
	CP   (HL)   	;          
	CP   (IX+08)	;          
	CP   (IY+09)	;          
	CP   A      	;          
	CP   B      	;          
	CP   C      	;          
	CP   D      	;          
	CP   E      	;          
	CP   H      	;          
	CP   L      	;          
	CP   3      	;          
	CPD  ;      	;          
	CPDR ;      	;          
	CPI  ;      	;          
	CPIR ;      	;          
	CPL  ;      	;          
	DAA  ;      	;          
	DEC  (HL)   	;          
	DEC  (IX+10)	;          
	DEC  (IY+11)	;          
	DEC  A      	;          
	DEC  B      	;          
	DEC  BC     	;          
	DEC  C      	;          
	DEC  D      	;          
	DEC  DE     	;          
	DEC  E      	;          
	DEC  H      	;          
	DEC  HL     	;          
	DEC  IX     	;          
	DEC  IY     	;          
	DEC  L      	;          
	DEC  SP     	;          
	DI   ;      	;          
	EI   ;      	;          
	EX   (SP)   	,	HL     
	EX   (SP)   	,	IX     
	EX   (SP)   	,	IY     
;	EX   AF     	,	AF`    
	EX   AF     	,	AF'    
	EX   DE     	,	HL     
	EXX  ;      	;          
	HALT ;      	;          
	IM   0      	;          
	IM   1      	;          
	IM   2      	;          
	IN   A      	,	(C)    
	IN   A      	,	(3)    
	IN   B      	,	(C)    
	IN   C      	,	(C)    
	IN   D      	,	(C)    
	IN   E      	,	(C)    
	IN   H      	,	(C)    
	IN   L      	,	(C)    
	IN   (C)    	;          
	INC  (HL)   	;          
	INC  (IX+12)	;          
	INC  (IY+13)	;          
	INC  A      	;          
	INC  B      	;          
	INC  BC     	;          
	INC  C      	;          
	INC  D      	;          
	INC  DE     	;          
	INC  E      	;          
	INC  H      	;          
	INC  HL     	;          
	INC  IX     	;          
	INC  IY     	;          
	INC  L      	;          
	INC  SP     	;          
	IND  ;      	;          
	INDR ;      	;          
	INI  ;      	;          
	INIR ;      	;          
	LD   (BC)   	,	A      
	LD   (DE)   	,	A      
	LD   (HL)   	,	A      
	LD   (HL)   	,	B      
	LD   (HL)   	,	C      
	LD   (HL)   	,	D      
	LD   (HL)   	,	E      
	LD   (HL)   	,	H      
	LD   (HL)   	,	L      
	LD   (HL)   	,	4      
	LD   (IX+14)	,	A      
	LD   (IX+14)	,	B      
	LD   (IX+14)	,	C      
	LD   (IX+14)	,	D      
	LD   (IX+14)	,	E      
	LD   (IX+14)	,	H      
	LD   (IX+14)	,	L      
	LD   (IX+15)	,	5      
	LD   (IY+16)	,	A      
	LD   (IY+16)	,	B      
	LD   (IY+16)	,	C      
	LD   (IY+16)	,	D      
	LD   (IY+16)	,	E      
	LD   (IY+16)	,	H      
	LD   (IY+16)	,	L      
	LD   (IY+17)	,	6      
	LD   (2023) 	,	A      
	LD   (2024) 	,	BC     
	LD   (2025) 	,	DE     
	LD   (2026) 	,	HL     
	LD   (2027) 	,	IX     
	LD   (2028) 	,	IY     
	LD   (2029) 	,	SP     
	LD   A      	,	(BC)   
	LD   A      	,	(DE)   
	LD   A      	,	(HL)   
	LD   A      	,	(IX+18)
	LD   A      	,	(IY+19)
	LD   A      	,	(2030) 
	LD   A      	,	A      
	LD   A      	,	B      
	LD   A      	,	C      
	LD   A      	,	D      
	LD   A      	,	E      
	LD   A      	,	H      
	LD   A      	,	L      
	LD   A      	,	I      
	LD   A      	,	7      
	LD   A      	,	R      
	LD   B      	,	(HL)   
	LD   B      	,	(IX+20)
	LD   B      	,	(IY+21)
	LD   B      	,	A      
	LD   B      	,	B      
	LD   B      	,	C      
	LD   B      	,	D      
	LD   B      	,	E      
	LD   B      	,	H      
	LD   B      	,	L      
	LD   B      	,	8      
	LD   BC     	,	(2031) 
	LD   BC     	,	1010   
	LD   C      	,	(HL)   
	LD   C      	,	(IX+22)
	LD   C      	,	(IY+23)
	LD   C      	,	A      
	LD   C      	,	B      
	LD   C      	,	C      
	LD   C      	,	D      
	LD   C      	,	E      
	LD   C      	,	H      
	LD   C      	,	L      
	LD   C      	,	9      
	LD   D      	,	(HL)   
	LD   D      	,	(IX+24)
	LD   D      	,	(IY+25)
	LD   D      	,	A      
	LD   D      	,	B      
	LD   D      	,	C      
	LD   D      	,	D      
	LD   D      	,	E      
	LD   D      	,	H      
	LD   D      	,	L      
	LD   D      	,	0      
	LD   DE     	,	(2032) 
	LD   DE     	,	1011   
	LD   E      	,	(HL)   
	LD   E      	,	(IX+26)
	LD   E      	,	(IY+27)
	LD   E      	,	A      
	LD   E      	,	B      
	LD   E      	,	C      
	LD   E      	,	D      
	LD   E      	,	E      
	LD   E      	,	H      
	LD   E      	,	L      
	LD   E      	,	1      
	LD   H      	,	(HL)   
	LD   H      	,	(IX+28)
	LD   H      	,	(IY+29)
	LD   H      	,	A      
	LD   H      	,	B      
	LD   H      	,	C      
	LD   H      	,	D      
	LD   H      	,	E      
	LD   H      	,	H      
	LD   H      	,	L      
	LD   H      	,	2      
	LD   HL     	,	(2033) 
	LD   HL     	,	1012   
	LD   I      	,	A      
	LD   IX     	,	(2034) 
	LD   IX     	,	1013   
	LD   IY     	,	(2035) 
	LD   IY     	,	1014   
	LD   L      	,	(HL)   
	LD   L      	,	(IX+30)
	LD   L      	,	(IY+31)
	LD   L      	,	A      
	LD   L      	,	B      
	LD   L      	,	C      
	LD   L      	,	D      
	LD   L      	,	E      
	LD   L      	,	H      
	LD   L      	,	L      
	LD   L      	,	3      
	LD   R      	,	A      
	LD   SP     	,	(2036) 
	LD   SP     	,	HL     
	LD   SP     	,	IX     
	LD   SP     	,	IY     
	LD   SP     	,	1015   
	LDD  ;      	;          
	LDDR ;      	;          
	LDI  ;      	;          
	LDIR ;      	;          
	NEG  ;      	;          
	NOP  ;      	;          
	OR   (HL)   	;          
	OR   (IX+32)	;          
	OR   (IY+33)	;          
	OR   A      	;          
	OR   B      	;          
	OR   C      	;          
	OR   D      	;          
	OR   E      	;          
	OR   H      	;          
	OR   L      	;          
	OR   4      	;          
	OTDR ;      	;          
	OTIR ;      	;          
	OUT  (C)    	,	A      
	OUT  (C)    	,	B      
	OUT  (C)    	,	C      
	OUT  (C)    	,	D      
	OUT  (C)    	,	E      
	OUT  (C)    	,	H      
	OUT  (C)    	,	L      
	OUT  (4)    	,	A      
	OUTD ;      	;          
	OUTI ;      	;          
	POP  AF     	;          
	POP  BC     	;          
	POP  DE     	;          
	POP  HL     	;          
	POP  IX     	;          
	POP  IY     	;          
	PUSH AF     	;          
	PUSH BC     	;          
	PUSH DE     	;          
	PUSH HL     	;          
	PUSH IX     	;          
	PUSH IY     	;          
	RES  2      	,	(HL)   
	RES  3      	,	(IX+34)
	RES  4      	,	(IY+35)
	RES  5      	,	A      
	RES  6      	,	B      
	RES  7      	,	C      
	RES  0      	,	D      
	RES  1      	,	E      
	RES  2      	,	H      
	RES  3      	,	L      
	RET  ;      	;          
	RET  C      	;          
	RET  M      	;          
	RET  NC     	;          
	RET  NZ     	;          
	RET  P      	;          
	RET  PE     	;          
	RET  PO     	;          
	RET  Z      	;          
	RETI ;      	;          
	RETN ;      	;          
	RL   (HL)   	;          
	RL   A      	;          
	RL   B      	;          
	RL   C      	;          
	RL   D      	;          
	RL   E      	;          
	RL   H      	;          
	RL   L      	;          
	RL   (IX+36)	;          
	RL   (IY+37)	;          
	RLA  ;      	;          
	RLC  (HL)   	;          
	RLC  (IX+38)	;          
	RLC  (IY+39)	;          
	RLC  A      	;          
	RLC  B      	;          
	RLC  C      	;          
	RLC  D      	;          
	RLC  E      	;          
	RLC  H      	;          
	RLC  L      	;          
	RLCA ;      	;          
	RLD  ;      	;          
	RR   (HL)   	;          
	RR   A      	;          
	RR   B      	;          
	RR   C      	;          
	RR   D      	;          
	RR   E      	;          
	RR   H      	;          
	RR   L      	;          
	RR   (IX+40)	;          
	RR   (IY+41)	;          
	RRA  ;      	;          
	RRC  (HL)   	;          
	RRC  (IX+42)	;          
	RRC  (IY+43)	;          
	RRC  A      	;          
	RRC  B      	;          
	RRC  C      	;          
	RRC  D      	;          
	RRC  E      	;          
	RRC  H      	;          
	RRC  L      	;          
	RRCA ;      	;          
	RRD  ;      	;          
	RST  $00    	;          
	RST  $08    	;          
	RST  $10    	;          
	RST  $18    	;          
	RST  $20    	;          
	RST  $28    	;          
	RST  $30    	;          
	RST  $38    	;          
	SBC  A      	,	(HL)   
	SBC  A      	,	(IX+44)
	SBC  A      	,	(IY+45)
	SBC  A      	,	A      
	SBC  A      	,	B      
	SBC  A      	,	C      
	SBC  A      	,	D      
	SBC  A      	,	E      
	SBC  A      	,	H      
	SBC  A      	,	L      
	SBC  A      	,	5      
	SBC  HL     	,	BC     
	SBC  HL     	,	DE     
	SBC  HL     	,	HL     
	SBC  HL     	,	SP     
	SCF  ;      	;          
	SET  4      	,	(HL)   
	SET  5      	,	(IX+46)
	SET  6      	,	(IY+47)
	SET  7      	,	A      
	SET  0      	,	B      
	SET  1      	,	C      
	SET  2      	,	D      
	SET  3      	,	E      
	SET  4      	,	H      
	SET  5      	,	L      
	SLA  (HL)   	;          
	SLA  (IX+48)	;          
	SLA  (IY+49)	;          
	SLA  A      	;          
	SLA  B      	;          
	SLA  C      	;          
	SLA  D      	;          
	SLA  E      	;          
	SLA  H      	;          
	SLA  L      	;          
	SLL  (HL)   	;          
	SLL  (IX+50)	;          
	SLL  (IY+51)	;          
	SLL  A      	;          
	SLL  B      	;          
	SLL  C      	;          
	SLL  D      	;          
	SLL  E      	;          
	SLL  H      	;          
	SLL  L      	;          
	SRA  (HL)   	;          
	SRA  (IX+52)	;          
	SRA  (IY+53)	;          
	SRA  A      	;          
	SRA  B      	;          
	SRA  C      	;          
	SRA  D      	;          
	SRA  E      	;          
	SRA  H      	;          
	SRA  L      	;          
	SRL  (HL)   	;          
	SRL  (IX+54)	;          
	SRL  (IY+55)	;          
	SRL  A      	;          
	SRL  B      	;          
	SRL  C      	;          
	SRL  D      	;          
	SRL  E      	;          
	SRL  H      	;          
	SRL  L      	;          
	SUB  (HL)   	;          
	SUB  (IX+56)	;          
	SUB  (IY+57)	;          
	SUB  A      	;          
	SUB  B      	;          
	SUB  C      	;          
	SUB  D      	;          
	SUB  E      	;          
	SUB  H      	;          
	SUB  L      	;          
	SUB  6      	;          
	XOR  (HL)   	;          
	XOR  (IX+58)	;          
	XOR  (IY+59)	;          
	XOR  A      	;          
	XOR  B      	;          
	XOR  C      	;          
	XOR  D      	;          
	XOR  E      	;          
	XOR  H      	;          
	XOR  L      	;          
	XOR  7      	;          

.dw $