#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_SIZE 100
#define RADIX 16     // hexadecimal (4 bits)
#define BITS 32      // total bits in a 4-byte int

// Function to perform radix sort on signed integers
void radix_sort(int arr[], int n) {
    int output[MAX_SIZE];
    int count[RADIX];
    unsigned int mask = 0xF;  // mask for 4 bits

    // Offset all integers by flipping the sign bit
    // This allows correct sorting of negative numbers when treated as unsigned
    unsigned int temp[MAX_SIZE];
    for (int i = 0; i < n; i++) {
        temp[i] = (unsigned int)(arr[i] ^ 0x80000000);
    }

    // Process 4 bits at a time (8 passes for 32 bits)
    for (int shift = 0; shift < BITS; shift += 4) {
        // Initialize bucket counts
        for (int i = 0; i < RADIX; i++)
            count[i] = 0;

        // Count occurrences of each nibble
        for (int i = 0; i < n; i++)
            count[(temp[i] >> shift) & mask]++;

        // Convert count[] to prefix sums
        for (int i = 1; i < RADIX; i++)
            count[i] += count[i - 1];

        // Build output array (stable sorting)
        for (int i = n - 1; i >= 0; i--) {
            int nibble = (temp[i] >> shift) & mask;
            output[--count[nibble]] = temp[i];
        }

        // Copy back to temp[]
        for (int i = 0; i < n; i++)
            temp[i] = output[i];
    }

    // Undo the offset to get back signed integers
    for (int i = 0; i < n; i++) {
        arr[i] = (int)(temp[i] ^ 0x80000000);
    }
}

int main(void) {
    int n;
    int arr[MAX_SIZE];

    // Read number of integers
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_SIZE) {
        return 1; // invalid input
    }

    // Read n integers
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            return 1;
        }
    }

    // Sort using radix sort
    radix_sort(arr, n);

    // Output sorted integers, one per line
    for (int i = 0; i < n; i++) {
        printf("%d\n", arr[i]);
    }

    return 0;
}

