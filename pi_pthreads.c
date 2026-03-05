#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef unsigned long long ull;

struct targs {
    int tid;
    int nthreads;
    ull nterms;
    double *partials; /* pointer to array of size nthreads */
};

void *worker(void *arg) {
    struct targs *a = (struct targs *)arg;
    int tid = a->tid;
    int nthreads = a->nthreads;
    ull n = a->nterms;
    ull chunk = n / nthreads;
    ull start = (ull)tid * chunk + 1;        /* i from 1..n */
    ull end   = start + chunk - 1;
    double partial = 0.0;
    for (ull i = start; i <= end; ++i) {
        double x = ((double)i - 0.5) / (double)n;
        partial += 4.0 / (1.0 + x * x);
    }
    a->partials[tid] = partial;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_threads> <num_terms>\n", argv[0]);
        return 1;
    }
    int nthreads = atoi(argv[1]);
    if (nthreads <= 0) {
        fprintf(stderr, "num_threads must be > 0\n");
        return 1;
    }
    ull nterms = strtoull(argv[2], NULL, 10);
    if (nterms == 0) {
        fprintf(stderr, "num_terms must be > 0\n");
        return 1;
    }
    if (nterms % nthreads != 0) {
        fprintf(stderr, "Warning: nterms (%llu) not divisible by num_threads (%d). Using integer division.\n",
                (unsigned long long)nterms, nthreads);
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * nthreads);
    struct targs *args = malloc(sizeof(struct targs) * nthreads);
    double *partials = calloc(nthreads, sizeof(double));
    if (!threads || !args || !partials) {
        perror("malloc");
        return 1;
    }

    /* record start time */
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (int t = 0; t < nthreads; ++t) {
        args[t].tid = t;
        args[t].nthreads = nthreads;
        args[t].nterms = nterms;
        args[t].partials = partials;
        int rc = pthread_create(&threads[t], NULL, worker, &args[t]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed for thread %d\n", t);
            return 1;
        }
    }

    /* join and combine */
    double sum = 0.0;
    for (int t = 0; t < nthreads; ++t) {
        pthread_join(threads[t], NULL);
        sum += partials[t];
    }

    double pi = sum / (double)nterms;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) * 1e-9;

    printf("pi approximated = %.15f\n", pi);
    printf("error = %.15f\n", fabs(pi - M_PI));
    printf("threads = %d, terms = %llu, elapsed = %.6f seconds\n",
           nthreads, (unsigned long long)nterms, elapsed);

    free(threads);
    free(args);
    free(partials);
    return 0;
}

