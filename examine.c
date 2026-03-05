#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

/* ---- Prints out the memory contents related to argv[] in hexadecimal format ---- */
void print_bytes(unsigned char *addr, size_t len, const char *label) {
    printf("%-8s |", label);
    for (size_t i = 0; i < len; i++) {
        printf(" %02hhx", *(addr + i));
    }
    printf(" | %p\n", addr);
}

void print_memory(unsigned char *start, unsigned char *end) {
    uintptr_t aligned_start = (uintptr_t)start & ~0x7ULL;      // align down
    uintptr_t aligned_end = ((uintptr_t)end + 7ULL) & ~0x7ULL; // align up
    unsigned char *p = (unsigned char *)aligned_start;

    while ((uintptr_t)p < aligned_end) {
        printf("|");
        for (int i = 0; i < 8; i++) {
            if (p + i < (unsigned char *)aligned_end)
                printf("%02hhx ", p[i]);
            else
                printf("   ");
        }

        printf("| ");
        for (int i = 0; i < 8; i++) {
            if (p + i < (unsigned char *)aligned_end) {
                unsigned char c = p[i];
                if (isprint(c))
                    printf("%c ", c);
                else if (c == '\0')
                    printf("\\0 ");
                else
                    printf(". ");
            }
        }
        printf("| %p\n", p);
        p += 8;
    }
}

int main(int argc, char *argv[]) {
    printf("Memory examination of argv and command line arguments\n\n");

    /* ---- Part 1: memory contents of argv ---- */
    print_bytes((unsigned char *)&argv, sizeof(char **), "argv");

    printf("\n");

    /* ---- Part 2: contents of argv[i] pointers ---- */
    for (int i = 0; i < argc; i++) {
        char label[20];
        snprintf(label, sizeof(label), "argv[%d]", i);
        print_bytes((unsigned char *)&argv[i], sizeof(char *), label);
    }

    printf("\n");

    /* ---- Part 3: examine memory storing actual strings ---- */
    unsigned char *start = (unsigned char *)argv[0];
    unsigned char *end = (unsigned char *)argv[argc - 1] + strlen(argv[argc - 1]) + 1;

    print_memory(start, end);

    return 0;
}
