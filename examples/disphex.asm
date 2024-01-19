.nolist
#include "ti84pcse.inc"
#define start_row 0
#define start_col 0

.list
.org  userMem - 2
.db   tExtTok, tAsm84CCmp
;		LD   DE, start_col * $FF + start_row
;		LD   (CurRow), DE
	b_call(_HomeUp)
	LD   DE, 0

	LD   C, 42
	LD   B, 10
loop:
	INC  C              ; C holds our byte of interest

	LD   E, C           ; upper hex-digit
	SRL  E
	SRL  E
	SRL  E
	SRL  E
	LD   IX, hex_digits
	ADD  IX, DE
	LD   A, (IX)
	b_call(_PutC)

	LD   A, C           ; lower hex digit
	AND  $0F
	LD   E, A
	LD   IX, hex_digits
	ADD  IX, DE
	LD   A, (IX)
	b_call(_PutC)

	LD   A, ' '
	b_call(_PutC)

	DJNZ loop

	b_call(_GetKey)
	RET

hex_digits:
	.db "0123456789ABCDEF"

.end