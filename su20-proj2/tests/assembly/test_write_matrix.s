.import ../../src/write_matrix.s
.import ../../src/utils.s

.data
m0: .word 1, 2, 3, 4, 5, 6, 7, 8, 9 # MAKE CHANGES HERE TO TEST DIFFERENT MATRICES
file_path: .asciiz "//su20-proj2-starter-master/tests/outputs/test_write_matrix/student_write_outputs.bin"

.text
main:
    # Write the matrix to a file
	la s0 file_path
	la s1 m0
	mv a0 s0
	mv a1 s1
	li a2 3
    li a3 3
	jal ra write_matrix

    # Exit the program
	jal exit
