# file interrupts.s
.section ivt
.word isr_reset
.skip 2 # isr_error
.word isr_timer
.word isr_terminal
.skip 8
.extern my_start , my_counter
.section isr
.equ term_out, 0xFF00
.equ term_in, 0xFF02
.equ ascii_code, 84 # ascii(’T’)
# prekidna rutina za reset
#isr_reset:
jmp my_start
# prekidna rutina za tajmer
#isr_timer:
push r0
ldr r0, $ascii_code
str r0, term_out
pop r0
iret
# prekidna rutina za terminal
#isr_terminal:
push r0
push r1
ldr r0, term_in
str r0, term_out
ldr r0, %my_counter # pcrel
ldr r1, $1
add r0, r1
str r0, my_counter # abs
pop r1
pop r0
iret
.end