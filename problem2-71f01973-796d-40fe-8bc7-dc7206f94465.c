#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * Reads unsigned integers from matrix.bin
 * and prints their hex, decimal, and byte-level representations.
 */

int main() {
    FILE *fp;
    unsigned int value;
    unsigned char *bytes;
    int i;

    fp = fopen("matrix.bin", "rb");
    if (fp == NULL) {
        perror("Error opening matrix.bin");
        return 1;
    }

    while (fread(&value, sizeof(unsigned int), 1, fp) == 1) {
        printf("Integer: Hex: 0x%08X Dec: %u\n", value, value);

        bytes = (unsigned char *)&value;
        for (i = 3; i >= 0; i--) {
            unsigned char b = bytes[i];
            printf("Byte %d: Hex: 0x%02X Char: ", 4 - i, b);
            if (isprint(b))
                printf("%c\n", b);
            else
                printf("\\%d\n", b);
        }
        printf("\n");
    }

    fclose(fp);
    return 0;
}

