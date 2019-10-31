#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define COMPRESSED
#include "helper.h"

int main(int argc, char **argv) {
    const unsigned int n = argc == 2 ? (int) strtol(argv[1], NULL, 10) : 1000000;
    double elapsed = -MPI_Wtime();
    size_t *marked = calloc((n >> (LOG_BITS + 1u)) + 1, sizeof(size_t));
    for (unsigned i = 3; i * i <= n; ++i) {
        if (getBit(marked, i)) continue;
        for (unsigned j = i * i; j <= n; j += i << 1u) setBit(marked, j);
    }
    FILE *f = fopen("primes.txt", "w");
    writeInt(2, f);
    for (int i = 3; i <= n; i += 2)
        if (!getBit(marked, i)) writeInt(i, f);
    elapsed += MPI_Wtime();
    printf("%lf\n", elapsed);
    free(marked);
}