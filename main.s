# file: main.s

.section sekcija

.word 12, g, f, 0x22                               

.global x,y,z
.extern a,b,c

.global g

xchg r4, r5

.skip 10

int r2
ret

iret

add r1,r3

sub r1,r3
mul r1,r3
div r1,r3
cmp r1,r3

not r4
or r1,r3
xor r1,r3
test r1,r3
shl r1,r3
shr r1,r3

halt

.end

.word 23