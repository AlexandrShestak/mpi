#include <iostream>
#include <stdio.h>
#include <string.h>
#include "../installationDir/include/mpi.h"
//#include "mpi/mpi.h"

using namespace std;

int main(int argc, char **argv) {
    int myRank, size;
    MPI_Status status;
    int tag = 1;

    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &myRank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

    if (myRank != 0) {
        char str[100];
        sprintf(str, "Hello from process   %d", myRank);
        MPI_Send(str, (unsigned)strlen(str) + 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    } else {
        int receivedCount;
        char** receivedMessages = new char*[size];
        for (int i = 1 ; i < size ; i++) {
            MPI_Probe(MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &receivedCount);
            receivedMessages[status.MPI_SOURCE] = new char[receivedCount-1];
            MPI_Recv(receivedMessages[status.MPI_SOURCE], receivedCount, MPI_CHAR,
                     status.MPI_SOURCE, tag, MPI_COMM_WORLD, &status);

            printf("%s \n", receivedMessages[status.MPI_SOURCE]);
        }

        for (int i = 1 ; i < size ; i++) {
            printf("%s \n", receivedMessages[i]);
            delete[] receivedMessages[i];
        }
        delete[] receivedMessages;
    }
    MPI_Finalize();
    return 0;
}