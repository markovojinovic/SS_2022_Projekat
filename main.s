# file: main.s

.section sekcija

.global c

jmp %c
jne *10
jeq *c
jgt c

halt

.end


#037F307600