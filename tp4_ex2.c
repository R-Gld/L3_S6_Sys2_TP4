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
 * Ecrire un programme qui permet d’initialiser avec des valeurs aléatoires une matrice d’entiers, de manière concurrente, ligne par ligne<br>
 * Pour implémenter ce mécanisme, les éléments d’une même ligne seront initialisés par un thread (voir l’exercice 3 du TP3).<br>
 * Important. La donn´ee applicative doit ˆetre l’information principale manipul´ee par le programme principal. Les param`etres des routines sont des informations interm´ediaires secondaires.
 */
int main(const int argc, char ** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <min> <max> <line_size>\n!!: In total, there is line_size² value calculated.", argv[0]);
        return EXIT_FAILURE;
    }
    long min = convertWithCheck(argv[1]), max = convertWithCheck(argv[2]);

    if (max < min) {
        const long tmp = min;
        max = min;
        min = tmp;
    }

    const long line_size = convertWithCheck(argv[3]);

    long ** base_arr = allocate_arr(line_size);
    // malloc done.

    srand((unsigned int) time(NULL) ^ getpid());

    for (int i = 0; i < line_size; ++i) {
        // Create a thread per line.
        struct init_param init_param_value = {
            .min = min,
            .max = max,
            .arr_size = line_size
        };
        init_param_value.arr = base_arr[i];

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

/**
 * Function developed <a href="https://github.com/R-Gld/L3_S6_Sys2_TP3/blob/62523aedb01348e89335ea333437e414b11cd033/ex3.c#L79-L97">here</a>.
 */
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
