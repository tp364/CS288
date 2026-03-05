#include <stdio.h>
#include <stdlib.h>

/*
 * Reads a binary matrix (matrix.bin), converts it to CSR format,
 * and writes the CSR representation to matrix.txt
 */

int main() {
    FILE *fp;
    unsigned int n, m;
    unsigned int *matrix;
    unsigned int i, j, nnz = 0;

    fp = fopen("matrix.bin", "rb");
    if (fp == NULL) {
        perror("Error opening matrix.bin");
        return 1;
    }

    fread(&n, sizeof(unsigned int), 1, fp);
    fread(&m, sizeof(unsigned int), 1, fp);

    matrix = (unsigned int *)malloc(n * m * sizeof(unsigned int));
    if (matrix == NULL) {
        perror("malloc");
        fclose(fp);
        return 1;
    }

    fread(matrix, sizeof(unsigned int), n * m, fp);
    fclose(fp);

    for (i = 0; i < n * m; i++)
        if (matrix[i] != 0)
            nnz++;

    unsigned int *values = (unsigned int *)malloc(nnz * sizeof(unsigned int));
    unsigned int *col_idx = (unsigned int *)malloc(nnz * sizeof(unsigned int));
    unsigned int *row_start = (unsigned int *)malloc((n + 1) * sizeof(unsigned int));

    if (!values || !col_idx || !row_start) {
        perror("malloc");
        free(matrix);
        return 1;
    }

    unsigned int idx = 0;
    row_start[0] = 0;

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            unsigned int val = matrix[i * m + j];
            if (val != 0) {
                values[idx] = val;
                col_idx[idx] = j;
                idx++;
            }
        }
        row_start[i + 1] = idx;
    }

    FILE *out = fopen("matrix.txt", "w");
    if (out == NULL) {
        perror("Error creating matrix.txt");
        return 1;
    }

    fprintf(out, "dimensions %u %u\n", n, m);

    fprintf(out, "values");
    for (i = 0; i < nnz; i++)
        fprintf(out, " %u", values[i]);
    fprintf(out, "\n");

    fprintf(out, "column_indices");
    for (i = 0; i < nnz; i++)
        fprintf(out, " %u", col_idx[i]);
    fprintf(out, "\n");

    fprintf(out, "row_start");
    for (i = 0; i <= n; i++)
        fprintf(out, " %u", row_start[i]);
    fprintf(out, "\n");

    fclose(out);
    free(matrix);
    free(values);
    free(col_idx);
    free(row_start);

    printf("CSR representation successfully written to matrix.txt\n");
    return 0;
}
