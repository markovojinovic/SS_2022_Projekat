# file: main.s

.section sekcija

.word 22, g, f, 0x33

.global x,y,z
.extern a,b,c

.global g

xchg r4, r5

halt

.end
