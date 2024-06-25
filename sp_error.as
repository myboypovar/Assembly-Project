a1: .data 5,-2

.extern label
.entry label
.entry a1

.entry a2

mov r2,a1
not label

prn a2
mov r2,a2
not hello

