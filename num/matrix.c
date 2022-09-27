#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/* Generates a random double between low and high */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/* Generates a random matrix */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocates space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. `parent` should be set to NULL to indicate that
 * this matrix is not a slice. You should also set `ref_cnt` to 1.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. Return 0 upon success.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows <= 0 || cols <= 0)
    {
        return -1;
    }
    matrix *n = malloc(sizeof(matrix)); 
    
    if (n == NULL)
    {
        return 1;
    }
    n->rows = rows;
    n->cols = cols;
    n->data = calloc(rows*cols, sizeof(double));
    n->ref_cnt = 1;
    n->parent = NULL;

    mat = n;
    return 0;
}

/*
 * Allocates space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * Its data should point to the `offset`th entry of `from`'s data (you do not need to allocate memory)
 * for the data field. `parent` should be set to `from` to indicate this matrix is a slice of `from`.
 * You should return -1 if either `rows` or `cols` or both are non-positive or if any
 * call to allocate memory in this function fails. Return 0 upon success.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int offset, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows <= 0 || cols <= 0)
    {
        return -1;
    }
    matrix *n = malloc(sizeof(matrix));
    if (n == NULL)
    {
        return 1;
    }
    n->cols = cols;
    n->rows = rows;
    n->ref_cnt = 1;
    n->parent = from;
    n->data = from->data + offset;
    from->ref_cnt += 1;

    mat = n;
}

/*
 * You need to make sure that you only free `mat->data` if `mat` is not a slice and has no existing slices,
 * or if `mat` is the last existing slice of its parent matrix and its parent matrix has no other references
 * (including itself). You cannot assume that mat is not NULL.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat->ref_cnt == 1 || (mat->parent->ref_cnt == 2 && mat->parent->parent == NULL))
    {
        free(mat->data);
    }
    mat->parent->ref_cnt -= 1;
    free(mat);
}

/*
 * Returns the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return *(mat->data + (row - 1) * mat->cols + col -1);
}

/*
 * Sets the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    *(mat->data + (row - 1) * mat->cols + col -1) = val;
}

/*
 * Sets all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    int n = mat->rows * mat->cols;
    for (int i = 0; i < n; i++)
    {
        *(mat->data + n) = val; 
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    result->rows = mat1->rows;
    result->cols = mat1->cols;
    int n = mat1->rows * mat1->cols;
    for (int i = 0; i < n; i++)
    {
        result->data[i] = mat1->data[i] + mat2->data[i];
    }
    result->parent = NULL;
    result->ref_cnt = 1;
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    result->rows = mat1->rows;
    result->cols = mat1->cols;
    int n = mat1->rows * mat1->cols;
    for (int i = 0; i < n; i++)
    {
        result->data[i] = mat1->data[i] - mat2->data[i];
    }
    result->parent = NULL;
    result->ref_cnt = 1;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    int m, n, l;
    m = mat1->rows;
    if (mat1->cols != mat2->rows)
    {
        return 1;
    }
    n = mat1->cols;
    l = mat2->cols;
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < m ; i++)
        {
            for (int k = 0; k < l; l++)
            {
                result->data[k+i*l] += mat1->data[j+i*n] * mat2->data[k+j*l];
            }
        }
    }
    result->rows = m;
    result->cols = l;
    result->parent = NULL;
    result->ref_cnt = 1;
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    /* TODO: YOUR CODE HERE */
    int i,j,k,l;
    if (mat->cols != mat->rows)
    {
        return -1;
    }
    int n = mat->cols;
    double *buff = calloc(n*n, sizeof(double));
    int s = n*n;
    for (i=0; i < s; i++)
    {
        buff[i] = mat->data[i];
    }
    for (i=0; i < (pow-1); i++)
    {
        for (k = 0; k < n; k++)
        {
            for (j = 0; j < n; j++)
            {
                for (l = 0; l < n; l++)
                {
                    result->data[l+j*n] += buff[k+j*n] * mat->data[l+k*n];
                }
            }
        }
        for (k = 0; k < s; k++)
        {
            buff[k] = result->data[k];
        }
    }
    result->cols = result->rows = n;
    result->ref_cnt = 1;
    result->parent = NULL;
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    int i, m, n, s;
    m = mat->cols;
    n = mat->rows;
    s = m * n;
    for (i = 0; i < s; i++)
    {
        result->data[i] = mat->data[i] * -1;
    }
    result->cols = m; 
    result->rows = n; 
    result->parent = NULL;
    result->ref_cnt = 1;
    return 0; 
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    int i, m, n, s;
    m = mat->cols;
    n = mat->rows;
    s = m * n;
    for (i = 0; i < s; i++)
    {
        if (mat->data[i] < 0)
        {
            result->data[i] = mat->data[i] * -1;
        }
        else
        {
            result->data[i] = mat->data[i];
        }
    }
    result->cols = m; 
    result->rows = n; 
    result->parent = NULL;
    result->ref_cnt = 1;
    return 0; 
}

