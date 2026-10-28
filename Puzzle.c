#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FRAGS 2000
#define TIME_LIMIT_SEC 2.0

typedef struct {
    int id;
    char full[64];
    char start[3];
    char end[3];
    int used;
} Fragment;

Fragment* load_fragments(const char *filename, int *out_count);
void do_puzzle(Fragment *fragments, int count, int *best_chain, int *out_max_len);
void dfs(Fragment *fragments, int count, int current_id, 
         int (*next_ids)[MAX_FRAGS], int *next_count,
         int *current_chain, int current_len, 
         int *best_chain, int *max_len, clock_t start_time, int *time_out);

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
    if (best_chain == NULL) {
        printf("Memory allocation failed for best_chain.\n");
        free(fragments);
        return 1;
    }

    int max_len = 0;

    clock_t t_start = clock();
    do_puzzle(fragments, count, best_chain, &max_len);
    clock_t t_end = clock();

    double elapsed = (double)(t_end - t_start) / CLOCKS_PER_SEC;

    printf("\nExecution time: %.3f seconds\n", elapsed);
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

    int capacity = MAX_FRAGS;
    int count = 0;

    Fragment *fragments = (Fragment *)malloc(capacity * sizeof(Fragment));
    if (fragments == NULL) {
        fclose(file);
        *out_count = 0;
        return NULL;
    }

    char buffer[64];

    while (fscanf(file, "%63s", buffer) == 1) {
        int len = strlen(buffer);

        if (len < 4) continue;

        if (count >= capacity) {
            capacity *= 2;
            Fragment *temp = (Fragment *)realloc(fragments, capacity * sizeof(Fragment));
            if (temp == NULL) {
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

    static int next_ids[MAX_FRAGS][MAX_FRAGS];
    static int next_count[MAX_FRAGS];
    static int in_count[MAX_FRAGS];

    memset(next_ids, 0, sizeof(next_ids));
    memset(next_count, 0, sizeof(next_count));
    memset(in_count, 0, sizeof(in_count));

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            if (i != j && strcmp(fragments[i].end, fragments[j].start) == 0) {
                next_ids[i][next_count[i]] = j;
                next_count[i]++;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        for (int k = 0; k < next_count[i]; k++) {
            int target = next_ids[i][k];
            in_count[target]++;
        }
    }

    // 3. Жадібне сортування кандидатів
    for (int i = 0; i < count; i++) {
        for (int a = 0; a < next_count[i] - 1; a++) {
            for (int b = a + 1; b < next_count[i]; b++) {
                int id_a = next_ids[i][a];
                int id_b = next_ids[i][b];
                if (next_count[id_a] < next_count[id_b]) {
                    int temp = next_ids[i][a];
                    next_ids[i][a] = next_ids[i][b];
                    next_ids[i][b] = temp;
                }
            }
        }
    }

    int has_zero_in_count = 0;
    for (int i = 0; i < count; i++) {
        if (in_count[i] == 0) {
            has_zero_in_count = 1;
            break;
        }
    }

    int *current_chain = (int *)malloc(count * sizeof(int));
    if (current_chain == NULL) {
        *out_max_len = 0;
        return;
    }

    int max_len = 0;
    clock_t start_time = clock();
    int time_out = 0;

    for (int i = 0; i < count; i++) {
        if (time_out) break;

        if ((has_zero_in_count && in_count[i] == 0) || (!has_zero_in_count && next_count[i] > 0)) {
            fragments[i].used = 1;
            current_chain[0] = i;

            dfs(fragments, count, i, next_ids, next_count, 
                current_chain, 1, best_chain, &max_len, start_time, &time_out);

            fragments[i].used = 0;
        }
    }

    *out_max_len = max_len;
    free(current_chain);
}

void dfs(Fragment *fragments, int count, int current_id, 
         int (*next_ids)[MAX_FRAGS], int *next_count,
         int *current_chain, int current_len, 
         int *best_chain, int *max_len, clock_t start_time, int *time_out) 
{
    if (*time_out) return;


    static int check_counter = 0;
    if (++check_counter % 1000 == 0) {
        double elapsed = (double)(clock() - start_time) / CLOCKS_PER_SEC;
        if (elapsed >= TIME_LIMIT_SEC) {
            *time_out = 1;
            return;
        }
    }

    if (current_len > *max_len) {
        *max_len = current_len;
        for (int i = 0; i < current_len; i++) {
            best_chain[i] = current_chain[i]; 
        }
    }

    for (int k = 0; k < next_count[current_id]; k++) {
        if (*time_out) return;

        int candidate = next_ids[current_id][k];

        if (fragments[candidate].used == 0) {
            fragments[candidate].used = 1;
            current_chain[current_len] = candidate;

            dfs(fragments, count, candidate, next_ids, next_count, 
                current_chain, current_len + 1, best_chain, max_len, start_time, time_out);

            fragments[candidate].used = 0;
        }
    }
}