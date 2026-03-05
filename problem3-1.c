#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 1000
#define MAX_LINE 1000

/* 
 * Function: count_lines_in_file
 * -----------------------------
 * Counts the number of lines in a single text file using fgets().
 */
long count_lines_in_file(const char *filepath) {
    FILE *fp;
    char line[MAX_LINE];
    long count = 0;

    fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;
    }

    fclose(fp);
    return count;
}

/* 
 * Function: has_txt_extension
 * ---------------------------
 * Checks if a given filename has a ".txt" extension.
 * Uses strrchr() to find the last '.' character.
 */
int has_txt_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (dot != NULL && strcmp(dot, ".txt") == 0)
        return 1;
    else
        return 0;
}

/* 
 * Function: traverse_directory
 * ----------------------------
 * Recursively traverses a directory, counting total lines
 * in all regular files with a ".txt" extension.
 */
long traverse_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    char fullpath[MAX_PATH];
    long total = 0;

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        /* Skip current and parent directory entries */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &info) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            /* Recursive call for subdirectories */
            total += traverse_directory(fullpath);
        } 
        else if (S_ISREG(info.st_mode) && has_txt_extension(entry->d_name)) {
            /* Count lines in .txt files only */
            total += count_lines_in_file(fullpath);
        }
    }

    closedir(dir);
    return total;
}

/* 
 * main()
 * ------
 * Reads directory name from user and prints total line count.
 */
int main() {
    char path[MAX_PATH];
    long total_lines;

    printf("Enter directory path: ");
    scanf("%s", path);

    total_lines = traverse_directory(path);
    printf("Total number of lines in .txt files: %ld\n", total_lines);

    return 0;
}

