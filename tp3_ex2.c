#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* exec_func(void *num_print_ptr) {
    const int num_print = *(int *) num_print_ptr;
    for (int i = 1; i < num_print+1; ++i) {
        printf("id %lu: %d\n", pthread_self(), i);
        sleep(1);
    }
    return NULL;
}

int main(void) {
    printf("PID du programme: %d\n", getpid());

    const int num_print = 10000;
    pthread_t tid1, tid2;
    int err = pthread_create(&tid1, NULL, exec_func, (void *) &num_print);
    if (err != 0) {
        errno = err;
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    err = pthread_create(&tid2, NULL, exec_func, (void *) &num_print);
    if (err != 0) {
        errno = err;
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    err = pthread_join(tid1, NULL);
    if (err != 0) {
        errno = err;
        perror("pthread_join");
        return EXIT_FAILURE;
    }
    err = pthread_join(tid2, NULL);
    if (err != 0) {
        errno = err;
        perror("pthread_join");
        return EXIT_FAILURE;
    }

    printf("All threads terminated.\n");

    return EXIT_SUCCESS;
}
