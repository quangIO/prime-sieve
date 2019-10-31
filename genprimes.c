#include <stdlib.h>
#include <mpi.h>

#ifdef COMPRESSED
#undef COMPRESSED
#endif

// #define NOBROADCAST


#include "helper.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

static int calculateBegin(int rank, int n, int p) {
    return 2 + ((rank * (n - 1) / p));
}

#pragma clang diagnostic pop

int main(int argc, char **argv) {
    int id;
    int num_process;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    double elapsed = -MPI_Wtime();
    int n = argc == 2 ? (int) strtol(argv[1], NULL, 10) : 1000000;
    const int begin = calculateBegin(id, n, num_process);
    const int end = calculateBegin(id + 1, n, num_process) - 1;
    const unsigned int size = end - begin + 1;
    const unsigned int bufSize = 1 + (size >> LOG_BITS);
    size_t *marked = (size_t *) calloc(bufSize + (!id), sizeof(size_t));

#ifndef NOBROADCAST
    int cur = 0;
    int p = 2;
    while (p * p <= n) {
        int first = 0, t;
        if ((t = p * p - begin) > 0) first = t;
        else if ((t = begin % p)) first = p - t;
        for (int i = first; i < size; i += p) setBit(marked, i);

        if (unlikely(!id)) {
            while (getBit(marked, ++cur));
            p = cur + 2;
        }
        if (likely(num_process > 1)) MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }


    if (id) {
        MPI_Send(marked, (int) bufSize, MPI_UNSIGNED_LONG_LONG, 0, 1, MPI_COMM_WORLD);
    } else {
        FILE *f = fopen("primes.txt", "w");
        int offset = 2;
        for (int i = 0, j = offset; i < size; ++i)
            if (!getBit(marked, i))
                writeInt(offset = i + j, f);

        /*
        size_t **buf = (size_t **) malloc((num_process - 1) * sizeof(size_t *));
        for (int i = 0; i < num_process - 1; ++i) buf[i] = (size_t *) malloc(bufSize * sizeof(size_t));
        int m[num_process - 1];
        MPI_Status status;
        for (int i = 0; i < num_process - 1; ++i) {
            MPI_Recv(buf[i], (int) bufSize, MPI_UNSIGNED_LONG_LONG, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,
                     &status);
            m[status.MPI_SOURCE - 1] = i;
        }
        for (int k = 0; k < num_process - 1; ++k) {
            for (int i = 0, j = offset; i < size; ++i)
                if (!getBit(buf[m[k]], i))
                    writeInt(offset = i + j, f);
            free(buf[m[k]]);
        }
        free(buf);
         */
        for (int k = 1; k < num_process; ++k) {
            MPI_Recv(marked, (int) bufSize + 1, MPI_UNSIGNED_LONG_LONG, k, 1, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            // m[status.MPI_SOURCE - 1] = i;
            for (int i = 0, j = offset; i < size; ++i)
                if (!getBit(marked, i))
                    writeInt(offset = i + j, f);
        }
    }

#else
    int bound = (int) (sqrt(n) + .5);ls
    size_t *precompute = (size_t *) calloc(((unsigned) bound >> LOG_BITS) + 1, sizeof(size_t));

#endif
    elapsed += MPI_Wtime();
    double real_elapsed;
    MPI_Reduce(&elapsed, &real_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (!id)
        printf("%lf\n", real_elapsed);
    free(marked);
    MPI_Finalize();
}
