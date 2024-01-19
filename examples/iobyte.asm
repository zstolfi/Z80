.nolist
#include "ti84pcse.inc"
.list

.org  userMem - 2
.db   tExtTok, tAsm84CCmp
	JP   start

	;~~~~~~~~~~~~;
	;    DATA    ;
	;~~~~~~~~~~~~;

#define Default 3
#define S1      2
#define S2      1
#define S3      0

#define Bit0 0
#define Bit1 1
#define Eof  255
#define x    0 ; placeholder value

; https://www.youtube.com/watch?v=VdJqXqXYbvE
Transition_Table:
	;    S3    S2    S1    DF
	.db  x   , Bit0, Bit1, Eof ; S3
	.db  Bit0, x   , Bit1, Eof ; S2
	.db  Bit0, Bit1, x   , Eof ; S1
	.db  Eof , Bit0, Bit1, x   ; Default

State_Buffer:
	#define State_Buffer_Size 9
	.db S1, S3, S1, S3, S1, S2, S1, S3, Default

	;~~~~~~~~~~~~~~~;
	;    PROGRAM    ;
	;~~~~~~~~~~~~~~~;

start:
	b_call(_ClrWindow )   ; initialize window
	b_call(_HomeUp )
	LD   IX, CurRow
	#define nRow 0;
	#define nCol 1;
;		LD   (IX + nRow), 9
;		LD   (IX + nCol), 45

	LD   C, Default       ; C holds prev decode state
	LD   A, Default       ; A holds decode state

	LD   HL, State_Buffer
	LD   B , State_Buffer_Size
	loop:
		LD   A, (HL)
		CP   C
		JR   Z, loop      ; loop unitl state change
		PUSH HL
		CALL getNextState ; A = Transition_Table[C][A]
		POP  HL

		LD   C, (HL)
		INC  HL
		CP   Eof          ; break if Eof
		JR   Z, done
	print:
		ADD  A, '0'       ; print state
		b_call(_PutMap )
		INC  (IX + nCol)

		DJNZ loop
done:
	b_call(_GetKey )      ; pause
	RET

	;~~~~~~~~~~~~~~~~;
	;    ROUTINES    ;
	;~~~~~~~~~~~~~~~~;

getNextState:
	LD   HL, Transition_Table
	SLA  C
	SLA  C
	ADD  A, C
	LD   D, 0
	LD   E, A
	ADD  HL, DE
	LD   A, (HL)
	RET

.end