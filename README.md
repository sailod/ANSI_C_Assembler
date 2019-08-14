# ANSI_C_Assembler

This Assembler written in C ANSI,
1. Gets asm files as argument
2. Outputs 3 parsed machine files for each asm file

Example:

example.as:
```json
; file example.as

.entry LIST
.extern W
.define sz = 2
MAIN:	mov r3, LIST[sz]
LOOP:	jmp W
		prn #-5
		mov STR[5], STR[2]
		sub r1, r4
		cmp K, #sz
		bne W
		L1:	inc L3
.entry LOOP
		bne LOOP
END:	stop
.define len = 4
STR: .string "abcdef"
LIST: .data 6, -9 , len
K: .data 22
.extern L3
```
example.ent:
```json
LOOP	104
LIST	232
```
example.ext:
```json
W	119
L3	121
```
example.ob:
```json
100	****!%*
101	******!
102	***!%%*
103	******%
104	**%##**
105	**#%%##
106	**!****
107	!!!!%!*
108	****%%*
109	*******
110	*****##
111	*******
112	******%
113	***!!!*
114	****!**
115	***##**
116	******%
117	*****%*
118	**%%#**
119	**#!#!#
120	**#!#**
121	**#!%##
122	**%%#**
123	******%
124	*******
225	***#%*#
226	***#%*%
227	***#%*!
228	***#%#*
229	***#%##
230	***#%#%
231	*******
232	*****#%
233	!!!!!#!
234	*****#*
235	****##%
```
