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


struct sum_param {
    long *arr;
    size_t arr_size;
};
void *sum_func(void *arg);



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

    long global_sum = 0;

    for (int sum_idx = 0; sum_idx < line_size; ++sum_idx) {
        struct sum_param sum_param_value = { .arr_size = line_size };
        sum_param_value.arr = base_arr[sum_idx];

        pthread_t tid;
        int err = pthread_create(&tid, NULL, sum_func, &sum_param_value);
        if (err != 0) {
            free_arr(base_arr, line_size);
            errno = err;
            perror("pthread_create");
            return EXIT_FAILURE;
        }

        void* ret_value_ptr;
        err = pthread_join(tid, &ret_value_ptr);
        if (err != 0) {
            free_arr(base_arr, line_size);
            errno = err;
            perror("pthread_join");
            return EXIT_FAILURE;
        }
        if (ret_value_ptr == NULL) {
            free_arr(base_arr, line_size);
            fprintf(stderr, "An error occur while trying to allocate the return value of the sum thread n%ld", tid);
            return EXIT_FAILURE;
        }
        const long ret_value = *(long *) ret_value_ptr;
        free(ret_value_ptr);
        global_sum += ret_value;
    }

    printf("Global Sum: %ld (Number of long added: %ld)\n", global_sum, (line_size*line_size));

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

void *sum_func(void *arg) {
    const struct sum_param param = *(struct sum_param *) arg;
    const long *arr = param.arr;
    const size_t arr_size = param.arr_size;
    // ReSharper disable once CppDFAMemoryLeak
    long *sum_value_ptr = malloc(sizeof(long));
    if (sum_value_ptr == NULL) return NULL;

    *sum_value_ptr = 0;
    for (int i = 0; i < arr_size; ++i) {
        *sum_value_ptr += arr[i];
    }
    pthread_exit(sum_value_ptr);
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
