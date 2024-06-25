; symbol errors
masd: .entry f!rst
asd: .extern mov
.extern mov:
.entry r5
.entry r5:

; label errors
mov: mov r4,r1
r1: prn r2
r12: hlt
!a: hlt
a!: hlt

; data errors
 h1: .data , +1 , +2
h2: .data -1,-2,
h3: .data -2, ,+1
h4: .data -2a, 1, 2
.data +-5,2.5

; string errors
h5: .string asd"
h6: .string "asd
h7: .string asd

; struct errors (mixing data and string)
h8: .struct 5,2, asd"
.struct 1,+2 "asd"

; instructions errors
a1: mov r1 , #1
not #1
lea r2 , #2
stop
hlt r2
cmp r2,r2, r3
jsr #1 , label
clr  ,
clr 





