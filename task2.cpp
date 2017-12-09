#include <iostream>
#include "../installationDir/include/mpi.h"

using namespace std;

double function(double x) {
    return (-3*x*x+2*x+9);  // 21 if a = -1, b = 2
    //return x+100;           // 20000  if a = -100, b = 100
}

int main(int argc, char **argv) {
    int myRank, size;
    MPI_Status status;
    int tag = 1;
    int n = 10;
    int r = 5;
    double a = -1;
    double b = 2;
    double integral = 0;
    int local_iterations_count = 1000000;

    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &myRank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

    if (myRank != 0) {
        int operation= 0;
        while (operation == 0) {
            MPI_Recv(&operation, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            if (operation == 0) {
                MPI_Recv(&a, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
                MPI_Recv(&b, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);

                double local_integral = (function(a) + function(b)) / 2;
                double height = (b - a) / local_iterations_count;
                for (int i = 1; i < local_iterations_count - 1; ++i) {
                    local_integral += function(a + height * i);
                }
                local_integral *= height;
                MPI_Send(&operation, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
                integral += local_integral;
            }
        }

    } else {
        double segment_size = (b - a) / n;
        double block_size = segment_size / r;

        for (int  i = 1 ; i < size ; i++) {
            double a_local = a;
            double b_local = a + block_size;
            int operation = 0;
            MPI_Send(&operation, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&a_local, 1, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
            MPI_Send(&b_local, 1, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
            a += block_size;
        }

        while (a < b) {
            int operation;
            MPI_Recv(&operation, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);

            double a_local = a;
            double b_local = a + block_size;

            MPI_Send(&operation, 1, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
            MPI_Send(&a_local, 1, MPI_DOUBLE, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
            MPI_Send(&b_local, 1, MPI_DOUBLE, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
            a += block_size;
        }
        for (int  i = 1 ; i < size ; i++) {
            int operation;
            MPI_Recv(&operation, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            operation = 1;
            MPI_Send(&operation, 1, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
        }
    }

    double* results = new double[size];
    MPI_Gather(&integral, 1, MPI_DOUBLE, results, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (myRank == 0) {
        double result = 0;
        for (int j = 1; j < size; ++j) {
            result += results[j];
        }
        cout << "Result: " << result;
    }

    MPI_Finalize();
    return 0;
}