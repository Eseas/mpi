#pragma ident "@(#)connectivity.c 1.2 99/09/01"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include <complex>

#include <string>


#include <mpi.h>

using namespace std;

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}


int
main(int argc, char **argv)
{
    MPI_Status	status;
    int		verbose = 1;
    int		rank;
    int		np;				/* number of processes in job */
    int		peer;
    int		i;

    int     imgWidth = 200;
    int     imgHeight = 100;
    int     imgWidthStep = 50;
    int     imgHeightStep = 50;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);


    // Eseas begin


    if (rank == 0) {
        int		i;

        int     width = 0;
        int     height = 0;

        int     totalPartitions = imgWidth / imgWidthStep * imgHeight / imgHeightStep;
        int     totalPartitionsGathered = 0;
        int     totalPartitionsScatterd = 0;




        // create tasks
        int j = 0;
        int** tasks = new int*[totalPartitions];
        for (int width = 0; width < imgWidth; width += imgWidthStep) {
            for (int height = 0; height < imgHeight; height += imgHeightStep) {
                tasks[j] = new int[4];

                tasks[j][0] = width;
                tasks[j][1] = height;
                tasks[j][2] = width+imgWidthStep;
                tasks[j][3] = height+imgHeightStep;

                printf("Task created. Task: {%d,%d,%d,%d}\n", tasks[j][0], tasks[j][1], tasks[j][2], tasks[j][3]);

                j++;
            }
        }
        printf("Finished chreating tasks: %d\n", totalPartitions);


        int **A;
        A = alloc_2d_int(imgWidth, imgHeight);


        for (i=1; i<np; i++) {

            for (int a = 0; a < imgWidth; a++) {
                for (int b = 0; b < imgHeight; b++) {
                    A[a][b] = 0;
                }
            }
            printf("MASTER_preparing_job: Slave_%d\n", i);
            int x0 = tasks[totalPartitionsScatterd][0];
            int y0 = tasks[totalPartitionsScatterd][1];
            int x1 = tasks[totalPartitionsScatterd][2];
            int y1 = tasks[totalPartitionsScatterd][3];

            int tsk[4];
            tsk[0] = tasks[totalPartitionsScatterd][0];
            tsk[1] = tasks[totalPartitionsScatterd][1];
            tsk[2] = tasks[totalPartitionsScatterd][2];
            tsk[3] = tasks[totalPartitionsScatterd][3];

            int area = (x1 - x0) * (y1 - y0);

            MPI_Send(&tsk, 4, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&(A[tsk[0]][tsk[2]]), area, MPI_INT, i, 1, MPI_COMM_WORLD);
            printf("MASTER_sent_job: Slave_%d Task: {%d,%d,%d,%d}\n", i, tsk[0], tsk[1], tsk[2], tsk[3]);

            totalPartitionsScatterd++;

//            MPI_Recv(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
//            MPI_Recv(&(A[tsk[0]][tsk[2]]), area, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
//
//            printf("MASTER_received_job: Slave_%d Task: {%d,%d,%d,%d}\n", status.MPI_SOURCE, tsk[0], tsk[1], tsk[2], tsk[3]);

        }


        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~ MASTER WENT TO RECV LOOP ~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        while (totalPartitionsGathered < totalPartitions) {


//            int **A;
//            A = alloc_2d_int(imgWidth, imgHeight);



            int tsk[4];
            MPI_Recv(&tsk, 4, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);


            int x0 = tsk[0];
            int y0 = tsk[1];
            int x1 = tsk[2];
            int y1 = tsk[3];

            int area = (x1 - x0) * (y1 - y0);
            printf("MASTER_received_task: Slave_%d Area: %d Task: {%d,%d,%d,%d}\n", status.MPI_SOURCE, area, tsk[0], tsk[1], tsk[2], tsk[3]);


            MPI_Recv(&(A[x0][x0]), area, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD, &status);

            printf("MASTER_received_job: Slave_%d Area: %d Task: {%d,%d,%d,%d}\n", status.MPI_SOURCE, area, tsk[0], tsk[1], tsk[2], tsk[3]);

//            printf("MASTER_IMAGE: \n");
//            for (int a = 0; a < imgWidth; a++) {
//                for (int b = 0; b < imgHeight; b++) {
//                    printf("%d", A[a][b]);
//                }
//                printf("\n");
//            }

            totalPartitionsGathered++;

            if (totalPartitionsScatterd < totalPartitions) {
                printf("MASTER_preparing_ADDITIONAL_job: Slave_%d\n", i);
                int x0 = tasks[totalPartitionsScatterd][0];
                int y0 = tasks[totalPartitionsScatterd][1];
                int x1 = tasks[totalPartitionsScatterd][2];
                int y1 = tasks[totalPartitionsScatterd][3];

                int tsk[4];
                tsk[0] = tasks[totalPartitionsScatterd][0];
                tsk[1] = tasks[totalPartitionsScatterd][1];
                tsk[2] = tasks[totalPartitionsScatterd][2];
                tsk[3] = tasks[totalPartitionsScatterd][3];

                int area = (x1 - x0) * (y1 - y0);

                MPI_Send(&tsk, 4, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
                MPI_Send(&(A[tsk[0]][tsk[2]]), area, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
                printf("MASTER_sent_ADDITIONAL_job: Slave_%d Task: {%d,%d,%d,%d}\n", i, tsk[0], tsk[1], tsk[2], tsk[3]);

                totalPartitionsScatterd++;
            }

        }


    } else if (rank != 0) {
        while(1) {
            int **A;
            int **B;
            A = alloc_2d_int(imgWidth, imgHeight);



            for (int a = 0; a < 10; a++) {
                for (int b = 0; b < 10; b++) {
                    A[a][b] = 0;
                }
            }

            int tsk[4];

            int area;



            MPI_Recv(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            printf("SLAVE_%d_received_task: {%d,%d,%d,%d}\n", rank, tsk[0], tsk[1], tsk[2], tsk[3]);

            int x0 = tsk[0];
            int y0 = tsk[1];
            int x1 = tsk[2];
            int y1 = tsk[3];

            B = alloc_2d_int(x1-x0,x1-x0);

            area = (x1 - x0) * (y1 - y0);
            printf("Area: %d\n", area);

            MPI_Recv(&(B[0][0]), area, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
//            MPI_Recv(&(B[x0][y0]), area, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

            printf("Received\n");

//        MPI_Send(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);
//        MPI_Send(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);
            string slaveJob1 = "";
            for (int row = x0; row < x1; row++) {
                for (int column = y0; column < y1; column++) {
//                printf("%c", A[a][b]);
//                    B[a][b] = rank;
                    std::complex<float> z, c = {
//                            (float)column * 2 / y1 - 1.5f,
//                            (float)row * 2 / x1 - 1
//                            column,
                            ((float)column - imgHeight/2) / (imgHeight/2),
                    ((float)row - imgWidth/2) / (imgWidth/4)
                    };
                    int iteration = 0;
                    while(abs(z) < 2 && ++iteration < 100)
                        z = pow(z, 2) + c;
//                    std::cout << (iteration == 100 ? '#' : '.');
                    slaveJob1 +=  (iteration == 100 ? '#' : '.');
                }
//                printf("\n");
                slaveJob1 += "\n";
            }

            cout << slaveJob1;
//
//            for (int a = x0; a < x1; a++) {
//                for (int b = y0; b < y1; b++) {
////                printf("%c", A[a][b]);
//                    B[a][b] = rank;
//                }
//            }

//        for (int a = 0; a < 5; a++) {
//            for (int b = 0; b < 5; b++) {
//                A[a][b] = '?';
//            }
//        }
//            string slaveJob = "";
//            printf("Slave made: imgWidth: %d\n", imgWidth);
//            for (int a = 0; a < x1-x0; a++) {
//                for (int b = 0; b < y1-y0; b++) {
//                    B[a][b] = rank;
////                    printf("%d", B[a][b]);
//                    char buffer[2];
//                    sprintf(buffer, "%d", B[a][b]);
//                    slaveJob += buffer;
//                }
//                slaveJob += "\n";
//            }

//            cout << slaveJob;
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

//            printf("Slave made: imgWidth: %d\n", imgWidth);
//            for (int a = 0; a < imgWidth; a++) {
//                for (int b = 0; b < imgHeight; b++) {
//                    printf("%d", A[a][b]);
//                }
//                printf("\n");
//            }

            MPI_Send(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);

            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            printf("Slave sent its task back\n");
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

            MPI_Send(&(B[0][0]), area, MPI_INT, 0, 2, MPI_COMM_WORLD);
//            MPI_Send(&(B[3][0]), 20, MPI_INT, 0, 2, MPI_COMM_WORLD);

            printf("SLAVE_%d_submitted_job: {%d,%d,%d,%d}\n", rank, tsk[0], tsk[1], tsk[2], tsk[3]);
        }

    }

    MPI_Finalize();
    return 0;
}
