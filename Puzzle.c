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
void do_puzzle(Fragment *fragments, int count, int *best_chain, int *out_max_len);
void dfs(Fragment *fragments, int count, int current_id, int *current_chain, int current_len, 
         int *best_chain, int *max_len);

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


    int *best_chain = (int *)malloc(count * sizeof(int));
    int max_len = 0;

    do_puzzle(fragments, count, best_chain, &max_len);

    printf("Found longest chain of length %d!\n", max_len);
    for (int i = 0; i < max_len; i++) {
        int id = best_chain[i];
        printf("%s ", fragments[id].full);
    }
    printf("\n");

    free(best_chain);
    free(fragments);
    printf("Loaded %d fragments successfully.\n", count);
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

void do_puzzle(Fragment *fragments, int count, int *best_chain, int *out_max_len) {
    if (count == 0) {
        *out_max_len = 0;
        return;
    }

    int *current_chain = (int *)malloc(count * sizeof(int));
    if (current_chain == NULL) {
        printf("Error: Memory allocation failed in do_puzzle\n");
        *out_max_len = 0;
        return;
    }

    int max_len = 0;

    for (int i = 0; i < count; i++) {
        fragments[i].used = 1;
        current_chain[0] = i;
        dfs(fragments, count, i, current_chain, 1, best_chain, &max_len);

        fragments[i].used = 0;
    }

    *out_max_len = max_len;

    free(current_chain);
}
    /*У нас є масив масивів чисел з їх порядковим номером,
    першими/останніми двома цифрами, повним числом та прапором використане число чи ні */ 

    /*Тепер потрібно зробити цикл, що  та роблячи розгалуження там, де є 2+ варіанти,
     перше розгалуження це спроба обрати як перший елемент кожне з чисел, у ході цього створюються "рекорди"  */


void dfs(Fragment *fragments, int count, int current_id, int *current_chain, int current_len, 
         int *best_chain, int *max_len) { 

        if (current_len > *max_len) {
            *max_len = current_len;
            for (int i = 0; i < current_len; i++) {
                best_chain[i] = current_chain[i]; 
            }
        }

        for (int i = 0; i < count; i++) {
            if (fragments[i].used == 0 && strcmp(fragments[i].start, fragments[current_id].end) == 0) {
                
                fragments[i].used = 1;
                current_chain[current_len] = i;

                dfs(fragments, count, i, current_chain, current_len + 1, best_chain, max_len);

                fragments[i].used = 0;
            }
    }


}
