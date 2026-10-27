#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char full[64];
    char start[3];
    char end[3];
    int used;
} Fragment;

Fragment* load_fragments(const char *filename, int *out_count);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: enter the file's name! Example: .\\Puzzle.exe numbers.txt\n");
        return 1;
    }

    int count = 0;
    
    Fragment *fragments = load_fragments(argv[1], &count);

    if (fragments == NULL) {
        printf("Failed to load fragments.\n");
        return 1;
    }

    printf("Loaded %d fragments:\n", count);
    for (int i = 0; i < count; i++) {
        printf("[%d] Full: %s | Start: %s | End: %s\n", 
               fragments[i].id, fragments[i].full, fragments[i].start, fragments[i].end);
    }

    free(fragments);

    return 0;
}


Fragment* load_fragments(const char *filename, int *out_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Run into error while opening the file");
        *out_count = 0;
        return NULL;
    }

    int capacity = 142;
    int count = 0;

    Fragment *fragments = (Fragment *)malloc(capacity * sizeof(Fragment));
    if (fragments == NULL) {
        printf("The program couldn't get memory from the system\n");
        fclose(file);
        *out_count = 0;
        return NULL;
    }

    char buffer[64];

    while (fscanf(file, "%63s", buffer) == 1) {
        int len = strlen(buffer);

        if (len < 4) {
            printf("Notice that your file contains a number of less than 4 digits, it was skipped\n");
            continue;
        }

        if (count >= capacity) {
            capacity *= 2;
            Fragment *temp = (Fragment *)realloc(fragments, capacity * sizeof(Fragment));

            if (temp == NULL) {
                printf("Error: couldn't reallocate memory.\n");
                free(fragments);
                fclose(file);
                *out_count = 0;
                return NULL;
            }
            fragments = temp;
        }

        fragments[count].id = count;
        strncpy(fragments[count].full, buffer, sizeof(fragments[count].full) - 1);
        fragments[count].full[sizeof(fragments[count].full) - 1] = '\0';

        fragments[count].start[0] = buffer[0];
        fragments[count].start[1] = buffer[1];
        fragments[count].start[2] = '\0';

        fragments[count].end[0] = buffer[len - 2];
        fragments[count].end[1] = buffer[len - 1];
        fragments[count].end[2] = '\0'; 

        fragments[count].used = 0;

        count++;
    }

    fclose(file);

    *out_count = count;

    return fragments;
}