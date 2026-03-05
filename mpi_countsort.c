#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/stat.h>

#define MAXVAL 1000

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char filepath[1024];

    unsigned int *all_data = NULL;   /* only allocated on rank 0 */
    unsigned int *local_buf = NULL;
    unsigned long numints = 0;
    unsigned long chunk = 0;

    if (rank == 0) {
        printf("Enter file pathname: ");
        fflush(stdout);
        if (scanf("%1023s", filepath) != 1) {
            fprintf(stderr, "Failed to read filepath\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        /* get file size and read entire file */
        struct stat st;
        if (stat(filepath, &st) != 0) {
            perror("stat");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (st.st_size % sizeof(unsigned int) != 0) {
            fprintf(stderr, "File size not multiple of 4 bytes\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        numints = st.st_size / sizeof(unsigned int);
        if (numints == 0) {
            fprintf(stderr, "File is empty\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        all_data = (unsigned int *) malloc(numints * sizeof(unsigned int));
        if (!all_data) {
            perror("malloc");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        FILE *f = fopen(filepath, "rb");
        if (!f) {
            perror("fopen");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        size_t read = fread(all_data, sizeof(unsigned int), numints, f);
        if (read != numints) {
            fprintf(stderr, "fread read %zu of %lu\n", read, numints);
            fclose(f);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fclose(f);

        /* compute chunk size and broadcast */
        if (numints % size != 0) {
            fprintf(stderr, "Number of integers (%lu) not divisible by number of processes (%d)\n",
                    numints, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    /* Broadcast numints to all ranks */
    MPI_Bcast(&numints, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    chunk = numints / size;

    /* allocate local buffer on each rank */
    local_buf = (unsigned int *) malloc(chunk * sizeof(unsigned int));
    if (!local_buf) {
        perror("malloc local_buf");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* Scatter the data from rank 0 to all ranks */
    MPI_Scatter(all_data, (int)chunk, MPI_UNSIGNED,
                local_buf, (int)chunk, MPI_UNSIGNED,
                0, MPI_COMM_WORLD);

    /* Each process builds local histogram */
    int local_count[MAXVAL];
    for (int i = 0; i < MAXVAL; ++i) local_count[i] = 0;

    unsigned long local_sum = 0;
    for (unsigned long i = 0; i < chunk; ++i) {
        unsigned int v = local_buf[i];
        if (v >= MAXVAL) {
            fprintf(stderr, "Rank %d found value %u out of range [0,%d]\n", rank, v, MAXVAL-1);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        local_count[v]++;
        local_sum += v;
    }

    /* Optional: get global sum for verification */
    unsigned long global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    /* Reduce histograms into global histogram on rank 0 */
    int global_count[MAXVAL];
    MPI_Reduce(local_count, global_count, MAXVAL, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        /* Reconstruct sorted array */
        unsigned int *sorted = (unsigned int *) malloc(numints * sizeof(unsigned int));
        if (!sorted) {
            perror("malloc sorted");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        unsigned long idx = 0;
        for (int val = 0; val < MAXVAL; ++val) {
            int cnt = global_count[val];
            for (int j = 0; j < cnt; ++j) {
                sorted[idx++] = (unsigned int)val;
            }
        }
        if (idx != numints) {
            fprintf(stderr, "Reconstruction mismatch: wrote %lu but expected %lu\n", idx, numints);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        /* Overwrite same file with sorted data */
        FILE *fw = fopen(filepath, "wb");
        if (!fw) {
            perror("fopen write");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        size_t wrote = fwrite(sorted, sizeof(unsigned int), numints, fw);
        if (wrote != numints) {
            fprintf(stderr, "fwrite wrote %zu of %lu\n", wrote, numints);
            fclose(fw);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fclose(fw);

        printf("Sorted %lu integers and wrote back to '%s'\n", numints, filepath);
        printf("Total (sum) of values = %lu\n", global_sum);

        free(sorted);
        free(all_data);
    }

    free(local_buf);

    MPI_Finalize();
    return 0;
}

