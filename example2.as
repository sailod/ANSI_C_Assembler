; file example.as

.entry LIST
.extern W
.define sz4 = 435
MAIN:	mov r3, LIST[sz4]
LOOP:	jmp W
		prn #-5
		mov STR[5], STR[2]
		sub r1, r4
		cmp K, #sz4
		bne W
		L1:	inc L3
.entry LOOP
		bne LOOP
END:	stop
.define len = 4
STR: .string "abcddferef"
LIST: .data 6, -9 , len
K: .data 22
.extern L3