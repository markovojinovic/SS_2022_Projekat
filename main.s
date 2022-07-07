# file: main.s

.extern mathAdd, mathSub, mathMul, mathDiv # neki komentar

.global my_start
.global value0, value1, value2, value3, value4, value5, value6
.section my_code

.word 0x43, c, b, a, 13
.word 0x1, d, f

.section neka_se

.word 0x12, cc, dd

.global c, b, a

.end
