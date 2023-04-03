; made by alex
MAIN: 	mov 	r3 ,LENGTH

.data 100		, 25  , +7  , -12
	mcr macro1
		inc 	r2
		mov 	A, r1
		rts
	endmcr
.extern    LINE
LOOP: 	jmp 	L1(#-1,r6)
	macro1
	mcr macro2
		sub 	r1, r4
		bne 	END
	endmcr
.entry   STR
	macro1
	prn 	#-5
	cmp   #-32  , K
	mcr macro3
		red 	r3
		jsr 	K
		jmp	    LINE
	endmcr
K:	bne 	LOOP(r4,r3) 
	add #-78, r0
.string   "hello"
	jmp 	SHALOM(LINE,LOOP)
	macro2
L1: 	inc K
aaa:	bne LOOP(K,STR)
	macro1
	macro2
END: 	stop
HELLO:      not r7
STR: 	.string "cbasfg54"
macro3
LENGTH: .data 6,-9,15
	sub #22, aaa
	lea HELLO, r5
Num:        .data 256
.extern   SHALOM
	clr r4
X:	dec Num
	red r6
	jsr    aaa(#75,X)
A:	inc X
.entry 		LOOP
	bne SHALOM(r2,STR)
	inc STR
	dec SHALOM
	stop
; end of file	

