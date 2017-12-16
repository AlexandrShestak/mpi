#include <iostream>
#include <stdio.h>
#include "../installationDir/include/mpi.h"

using namespace std;

int main(int argc, char **argv) {
    int myRank, size;
    MPI_Status status;
    int tag = 1;

    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &myRank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

    int N1 = 100;  // rows
    int N2 = 100;  // columns
    int r = 20;

    int process_rows_count = N1 / size;
    int blocks_count = N2 / r;

    int* buffer = new int[r];
    int* process_matrix_block = new int[process_rows_count * N2];

    int previous_process_rank;
    int next_process_rank;
    for (int i = 0 ; i < size ; i++) {
        previous_process_rank = myRank == 0 ? MPI_PROC_NULL : myRank-1;
        next_process_rank = myRank == size-1 ? MPI_PROC_NULL : myRank+1;
    }

    if (myRank == 0) {
        for (int i = 0; i < r; ++i) {
            buffer[i] = 0;
        }
    }

    MPI_Request recv_request = MPI_REQUEST_NULL;
    MPI_Request send_request = MPI_REQUEST_NULL;
    for (int i = 0; i < blocks_count; ++i) {
        if (i == 0) {
            MPI_Recv(buffer, r, MPI_INT, previous_process_rank, tag, MPI_COMM_WORLD, &status);
        } else {
            MPI_Wait(&recv_request, &status);
        }

        for (int j = 0; j < r; ++j) {
            process_matrix_block[i * r + j] = buffer[j] + 1;
        }


        MPI_Irecv(buffer, r, MPI_INT, previous_process_rank, tag, MPI_COMM_WORLD, &recv_request);

        for (int j = 1; j < process_rows_count ; ++j) {
            for (int k = 0; k < r; ++k) {
                process_matrix_block[j * N2 + i * r + k] = process_matrix_block[(j - 1) * N2 + i * r + k] + 1;
            }
        }

        MPI_Wait(&send_request, &status);
        MPI_Isend(process_matrix_block + (process_rows_count - 1) * N2 + i * r, r, MPI_INT, next_process_rank,
                  tag, MPI_COMM_WORLD, &send_request);
    }


    MPI_Barrier(MPI_COMM_WORLD);


    if (myRank == size - 1) {

        for (int k = 0; k < N2; ++k) {
            printf("%d ", process_matrix_block[(process_rows_count - 2) * N2 + k]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}