#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

long convertWithCheck(const char *arg);
void free_arr(long ** base_arr, long size);
long **allocate_arr(long size);

struct init_param {
    const long min, max;
    long *arr;
    size_t arr_size;
};

void *init_func(void *arg);



/**
 * Utiliser l’exercice précédent pour initialiser une matrice de mani`ere al´eatoire<br>
* Cr´eer un thread pour chaque ligne de la matrice qui calcule la somme des ´el´ements de la ligne correspondante et le programme principal calcule la somme finale, additionnant toutes les sommes partielles, des lignes, calcul´ees par les threads
 */
int main(const int argc, char ** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <min> <max> <line_size>\n!!: In total, there is line_size² value calculated.", argv[0]);
        return EXIT_FAILURE;
    }
    long min = convertWithCheck(argv[1]);
    long max = convertWithCheck(argv[2]);

    if (max < min) {
        const long tmp = min;
        max = min;
        min = tmp;
    }

    const long line_size = convertWithCheck(argv[3]);

    long ** base_arr = allocate_arr(line_size);
    // malloc done.

    srand((unsigned int) time(NULL) ^ getpid());

    for (int init_idx = 0; init_idx < line_size; ++init_idx) {
        // Create a thread per line.
        struct init_param init_param_value = {
            .min = min,
            .max = max,
            .arr_size = line_size
        };
        init_param_value.arr = base_arr[init_idx];

        pthread_t tid;
        int err = pthread_create(&tid, NULL, init_func, &init_param_value);
        if (err != 0) {
            free_arr(base_arr, line_size);
            errno = err;
            perror("pthread_create");
            return EXIT_FAILURE;
        }
        err = pthread_join(tid, NULL);
        if (err != 0) {
            free_arr(base_arr, line_size);
            errno = err;
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < line_size; ++i) {
        for (int j = 0; j < line_size; ++j) {
            printf("[%ld] ", base_arr[i][j]);
        }
        printf("\n");
    }

    free_arr(base_arr, line_size);
    return EXIT_SUCCESS;
}

void *init_func(void *arg) {
    const struct init_param param = *(struct init_param *) arg;
    long *arr = param.arr;
    const size_t arr_size = param.arr_size;
    const long min = param.min, max = param.max;

    for (long i = 0; i < arr_size; ++i) {
        arr[i] = min + random() % (max - min +1);
    }

    return NULL;
}

long **allocate_arr(const long size) {
    long **base_arr = malloc(sizeof(long *) * size);
    if (base_arr == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; ++i) {
        long *curr_arr = malloc(sizeof(long) * size);
        if (curr_arr == NULL) {
            for (int j = 0; j < i; ++j) {
                free(base_arr[j]); // free the array already allocated at this moment.
            }
            free(base_arr); // free base array.
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        base_arr[i] = curr_arr;
    }
    return base_arr;
}

void free_arr(long ** base_arr, const long size) {
    if (base_arr == NULL) return;
    for (long i = 0; i < size; ++i) {
        free(base_arr[i]);
    }
    free(base_arr);
}

long convertWithCheck(const char *arg) {
    char *endPointer;
    const long result = strtol(arg, &endPointer, 10);
    if ((errno == ERANGE && (result == LONG_MAX || result == LONG_MIN)) || (errno != 0 && result == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endPointer == arg) {
        fprintf(stderr, "No Digits were found\n");
        exit(EXIT_FAILURE);
    }

    if (*endPointer != '\0') {
        fprintf(stderr, "Further characters after number: %s\n", endPointer);
        exit(EXIT_FAILURE);
    }
    return result;
}
