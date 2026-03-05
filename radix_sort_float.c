#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void radix_sort(float *arr, int n) {
    unsigned int *src = (unsigned int *)arr;
    unsigned int *buf = (unsigned int *)malloc(n * sizeof(unsigned int));
    if (!buf) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    #define RADIX 16
    const int BITS = 32;
    const int SHIFT = 4;
    unsigned int mask = RADIX - 1;  // 0xF

    // Adjust for IEEE 754 ordering
    for (int i = 0; i < n; i++) {
        unsigned int x = src[i];
        if (x & 0x80000000)
            src[i] = ~x;
        else
            src[i] = x ^ 0x80000000;
    }

    // Perform 8 passes (32 bits / 4 bits per pass)
    for (int shift = 0; shift < BITS; shift += SHIFT) {
        int count[RADIX];
        memset(count, 0, sizeof(count));

        // Count occurrences
        for (int i = 0; i < n; i++)
            count[(src[i] >> shift) & mask]++;

        // Set up pointer array for buckets
        unsigned int *bucket[RADIX];
        bucket[0] = buf;
        for (int i = 1; i < RADIX; i++)
            bucket[i] = bucket[i - 1] + count[i - 1];

        // Distribute into buckets
        for (int i = 0; i < n; i++) {
            unsigned int val = src[i];
            int digit = (val >> shift) & mask;
            *(bucket[digit]++) = val;
        }

        // Swap src/buf for next pass
        unsigned int *temp = src;
        src = buf;
        buf = temp;
    }

    // Undo the earlier transformation
    for (int i = 0; i < n; i++) {
        unsigned int x = src[i];
        if (x & 0x80000000)
            src[i] = x ^ 0x80000000;
        else
            src[i] = ~x;
    }

    // Copy final result back if needed
    if ((void *)src != (void *)arr)
        memcpy(arr, src, n * sizeof(float));

    free(buf);
}

int main() {
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) {
        return 0;
    }

    float *arr = (float *)malloc(n * sizeof(float));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        if (scanf("%f", &arr[i]) != 1) {
            free(arr);
            return 0;
        }
    }

    radix_sort(arr, n);

    for (int i = 0; i < n; i++) {
        printf("%f\n", arr[i]);
    }

    free(arr);
    return 0;
}
