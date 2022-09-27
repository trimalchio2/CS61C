.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#   If any file operation fails or doesn't read the proper number of bytes,
#   exit the program with exit code 1.
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
#
# If you receive an fopen error or eof, 
# this function exits with error code 50.
# If you receive an fread error or eof,
# this function exits with error code 51.
# If you receive an fclose error or eof,
# this function exits with error code 52.
# ==============================================================================
read_matrix:
    # Prologue
	# sotre registers
    addi sp, sp, -36
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw s6, 24(sp)
    sw s7, 28(sp)
    sw ra, 32(sp)
	
	mv s0, a0
	mv s1, a1
	mv s2, a2
	
	# fopen
	mv a1, s0
	li a2, 0
	jal fopen
	blt a0, x0, exit_code50
	mv s5, a0
	
	# read arg1 rows
	mv a1, s5
	mv a2, s1
	li a3, 4
	jal fread
	blt a0, x0, exit_code51
	
	# read arg2 column
	mv a1, s5
	mv a2, s2
	li a3, 4
	jal fread
	blt a0, x0, exit_code51
	
	#malloc matrix
	lw t5, 0(s1)
	lw t6, 0(s2)
	mul t2, t5, t6
	#multiply the word byte
	slli t2, t2, 2
	mv a0, t2
	mv s7, t2
	jal malloc
	mv s3, a0
	
	li s6, 0
	#lw t2, 0(s1)
	#lw t3, 0(s2)
	#mul t2, t2, t3
	#slli t2, t2, 2

loop_start:
	#file descriptor
	mv a1, s5
	#pointer to the malloc
	mv a2, s3
	add a2, a2, s6
	mv a3, s7
	jal fread
	add s6, a0, s6
	blt s6, s7, loop_start

loop_end:
	mv a1, s5
	jal fclose
	blt a0, zero, exit_code52

    # Epilogue
	#mv t0, s3
	mv a0, s3
	mv a1 t5
	mv a1 t6
	lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw s6, 24(sp)
    lw s7, 28(sp)
    lw ra, 32(sp)
    addi sp, sp, 36

    
    ret
	
exit_code50:
	li a1, 50
	j exit2
	
exit_code51:
	li a1, 51
	j exit2
	
exit_code52:
	li a1, 52
	j exit2