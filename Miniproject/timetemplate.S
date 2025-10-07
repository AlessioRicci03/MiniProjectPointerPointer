# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)
#	addi	sp,sp,-4
#	sw	%reg,0(sp) 
#.end_macro

#.macro	POP (%reg)
#	lw	%reg,0(sp)
#	addi	sp,sp,4
#.end_macro
###################
# Macros for DTEK #
###################
#.macro	PUSH reg
#	addi sp,sp,-4
#	sw \reg,0(sp) 
#.endm

#.macro	POP reg
#	lw	\reg,0(sp)
#	addi	sp,sp,4
#.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x5957
timstr:	.space 6
	.text
	.globl timetemplate, tick, time2string, display_string, delay

# Function for displaying a string with a newline at the end	
display_string:
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra

timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 2		# ms
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

#########################################################
# Place for your functions: time2string, hex2asc, delay.#
#########################################################

time2string:
    addi sp, sp, -8
    sw ra, 4(sp)
    sw s0, 0(sp)

    mv s0, a0        # s0 = buffer pointer

    # minuter tiotal - MSN
    srli t0, a1, 12      # shift top nibble
    andi t0, t0, 0xF
    mv a0, t0
    jal hexasc
    sb a0, 0(s0)
    addi s0, s0, 1

    # minuter ental - nibble 2
    srli t0, a1, 8
    andi t0, t0, 0xF
    mv a0, t0
    jal hexasc
    sb a0, 0(s0)
    addi s0, s0, 1

    # colon
    li t0, 0x3A
    sb t0, 0(s0)
    addi s0, s0, 1

    # sekunder tiotal - nibble 3
    srli t0, a1, 4
    andi t0, t0, 0xF
    mv a0, t0
    jal hexasc
    sb a0, 0(s0)
    addi s0, s0, 1

    # sekunder ental - LSN
    andi t0, a1, 0xF
    mv a0, t0
    jal hexasc
    sb a0, 0(s0)
    addi s0, s0, 1

    # \0
    sb x0, 0(s0)

    lw ra, 4(sp)
    lw s0, 0(sp)
    addi sp, sp, 8
    jr ra


hex2asc:
	li	a0, 4		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	jr      ra

hexasc:
	andi a0, a0, 0xf
	
	li t0, 10
	bge a0, t0, conv2letter
	
	li t1, 0x30
	add a0, a0, t1
	ret
	

conv2letter:
	li t1, 0x41
	addi a0, a0, -10
	add a0, a0, t1
	ret

delay:
    addi sp, sp, -8
    sw   ra, 4(sp)
    sw   s0, 0(sp)

    mv   s0, a0          # ms counter

while_loop:
    blez s0, done_delay  # while(ms > 0)

    addi s0, s0, -1      # ms

    li   t0, 0           # i = 0
    li   t1, 5700000        # to be altered

for_loop:
    bge  t0, t1, while_loop
    addi t0, t0, 1
    j    for_loop

done_delay:
    lw   s0, 0(sp)
    lw   ra, 4(sp)
    addi sp, sp, 8
    jr   ra
