.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
#   If any file operation fails or doesn't write the proper number of bytes,
#   exit the program with exit code 1.
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
#
# If you receive an fopen error or eof, 
# this function exits with error code 53.
# If you receive an fwrite error or eof,
# this function exits with error code 54.
# If you receive an fclose error or eof,
# this function exits with error code 55.
# ==============================================================================
write_matrix:

    # Prologue
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
	
	mv s0, a0 #char* filename
	mv s1, a1 #int*
	mv s2, a2 #rows
	mv s3, a3 #columns
	
	#locate the argument
	mv a1, s0
	li a2, 1
	jal fopen
	blt a0, x0, exit_code53
	#file descriptor
	mv s5, a0
	
	#write the row information
	addi sp, sp, -4
	sw s2, 0(sp)
	mv a1, s5
	mv a2, sp
	li a3, 1
	li a4, 4
	jal fwrite
	blt a0, x0, exit_code54
	
	#write the column information
	sw s3, 0(sp)
	mv a1, s5
	mv a2, sp
	li a3, 1
	li a4, 4
	jal fwrite
	blt a0, x0, exit_code54
	addi sp, sp, 4
	
	#current index
	li s6, 0
	mul s7, s2, s3
	
loop_start:
	mv a1, s5
	
	slli t0, s6, 2
	add t0, s1, t0
	mv a2, t0
	
	mv a3, s7
	li a4, 4
	jal fwrite
	blt a0, x0, exit_code54
	add s6, s6, a0
	bne s6, s7, loop_start
	
	#close the file
	mv a1, s5
	jal fclose
	blt a0, x0, exit_code55
	
    # Epilogue
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

exit_code53:
	li a1, 53
	j exit2

exit_code54:
	li a1, 54
	j exit2

exit_code55:
	li a1, 55
	j exit2