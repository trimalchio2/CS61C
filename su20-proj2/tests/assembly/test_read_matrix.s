.import ../../src/read_matrix.s
.import ../../src/utils.s

.data
file_path: .asciiz "//su20-proj2-starter-master/tests/inputs/test_read_matrix/test_input.bin"

.text
main:
    # Read matrix into memory
	la t0 file_path
    mv a0 t0
    jal ra read_matrix

    # Print out elements of matrix
    li a1 3
    li a2 3
	jal ra print_int_array


    # Terminate the program
    jal exit