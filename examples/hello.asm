.nolist
#include "ti84pcse.inc"

.list
.org  userMem - 2
.db   tExtTok, tAsm84CCmp
	b_call(_ClrWindow)
	ld  hl, 0
	ld  (PenCol), hl
	ld  hl, msg
	b_call(_PutS)
	b_call(_NewLine)
	ret

msg:
	.db "Hello world!\0"

.end