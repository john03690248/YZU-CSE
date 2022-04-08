.globl main
.eqv MAX_FIBO	100	# Largest x + 1
.data
	endl:	.string"\n"
	Input0:	.string"Input a number:\n"
	Output0:	.string"The damage:\n"
.text	
main:	
	li s1, MAX_FIBO
	
	la a0, Input0
	li a7, 4
	ecall
	li a7, 5
	ecall
	bge a0, s1, done	# if x >= 100 done 
	bltz a0, done		# if x < 0 done
	
	jal fibo
	mv s2, a0	# store answer in s2 temporarily
	
	la a0, Output0
	li a7, 4
	ecall
	
	mv a0,s2	# store answer back to a0
	li a7, 1
	ecall
done:
	li a7, 10	# exit
	ecall
fibo:
	bltz a0, otherwise	# otherwise
	
	beqz a0, is_zero	# x == 0
	
	addi t0, a0, -1	
	beqz t0, is_one		# x == 1
	
	addi t0, a0, -11
	bltz t0, fibo1		# 1 < x <= 10
	
	addi t0, a0, -21	# 10 < x <= 20
	bltz t0, fibo2
	
	bge zero, zero, fibo3	# x > 20
otherwise:
	addi a0, zero, -1
	ret
is_zero:
	addi a0, zero, 1
	ret
is_one:
	addi a0, zero, 5
	ret
fibo1:
	addi sp, sp, -8		# make room for two 32-bit words on stack
	sw a0, 0(sp)		# save x
	sw ra, 4(sp)		# sava return address
	
	addi a0, a0, -1		# x - 1
	jal fibo		# cal fibo[x - 1]
	
	lw t0, 0(sp)		# get x
	sw a0, 0(sp)		# store fibo[x - 1]
	addi a0, t0, -2		# x - 2
	jal fibo		# cal fibo[x - 2]
	
	lw t0, 0(sp)		# get fibo[x - 1]
	add a0, a0, t0		# add fibo[x - 1] and fibo[x - 2] and put in a0
	
	lw ra, 4(sp)		# get return address
	addi sp, sp, 8		# clean up stack
	ret
fibo2:
	addi sp, sp, -8		# make room for two 32-bit words on stack
	sw a0, 0(sp)		# save x
	sw ra, 4(sp)		# sava return address
	
	addi a0, a0, -2		# x - 2
	jal fibo		# cal fibo[x - 2]
	
	lw t0, 0(sp)		# get x
	sw a0, 0(sp)		# store fibo[x - 2]
	addi a0, t0, -3		# x - 3
	jal fibo		# cal fibo[x - 3]
	
	lw t0, 0(sp)		# get fibo[x - 2]
	add a0, a0, t0		# add fibo[x - 2] and fibo[x - 3] and put in a0
	
	lw ra, 4(sp)		# get return address
	addi sp, sp, 8		# clean up stack
	ret
fibo3:
	addi sp, sp, -8		# make room for two 32-bit words on stack
	sw a0, 0(sp)		# save x
	sw ra, 4(sp)		# sava return address

	addi t0, zero, 5
	div a0, a0, t0		# get x / 5
	jal fibo		# cal fibo[x / 5]
	
	lw t0, 0(sp)		# get x
	slli t0, t0, 1		# get 2 * x
	add a0, a0, t0 		# add fibo[x / 5] and 2 * x and put in a0
	
	lw ra, 4(sp)		# get return address
	addi sp, sp, 8		# clean up stack
	ret
