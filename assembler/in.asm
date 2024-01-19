start:
	.org $8000
	.dw $
	LD a, b
	LD a, c
	LD a, d
	LD a, 45
	NOP
	LD b, (HL)
	LD b, (IX)
	LD b, (IX+0)
	LD b, (IX+100)
	.dw $