#include <complex>
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
    MPI_Status status;
    int	rank;
    int	np;				/* number of processes in job */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int debug;
    int gui;
    int maxIter;
    int imgCols;
    int imgRows;
    int imgColsStep;
    int imgRowsStep;

    std::istringstream iss0( argv[1] );
    std::istringstream iss1( argv[2] );
    std::istringstream iss2( argv[3] );
    std::istringstream iss3( argv[4] );
    std::istringstream iss4( argv[5] );
    std::istringstream iss5( argv[6] );
    std::istringstream iss6( argv[7] );

    iss0 >> debug;
    iss1 >> gui;
    iss2 >> maxIter;
    iss3 >> imgCols;
    iss4 >> imgRows;
    iss5 >> imgColsStep;
    iss6 >> imgRowsStep;




    if (rank == 0) {

        printf("Available workers (threads): %d\n"
               "Program starting:\n"
               "debug: %d; "
               "gui: %d; "
               "maxIter: %d; "
               "imgWidth: %d; "
               "imgHeight: %d; "
               "imgWidthStep: %d; "
               "imgHeightStep: %d; \n",
               np - 1,
               debug,
               gui,
               maxIter,
               imgCols,
               imgRows,
               imgColsStep,
               imgRowsStep);

        int totalPartitions = imgCols / imgColsStep * imgRows / imgRowsStep;
        int totalPartitionsGathered = 0;
        int totalPartitionsScattered = 0;


        // create tasks
        int j = 0;
        int** tasks = new int*[totalPartitions];
        for (int width = 0; width < imgCols; width += imgColsStep) {
            for (int height = 0; height < imgRows; height += imgRowsStep) {
                tasks[j] = new int[4];

                tasks[j][0] = width;
                tasks[j][1] = height;
                tasks[j][2] = width+imgColsStep;
                tasks[j][3] = height+imgRowsStep;

                if (debug) printf("Task created. Task: {%d,%d,%d,%d}\n", tasks[j][0], tasks[j][1], tasks[j][2], tasks[j][3]);

                j++;
            }
        }
        if (debug) printf("Finished creating tasks: %d\n", totalPartitions);


        int **A;
        A = alloc_2d_int(imgRows, imgCols);

        if (debug) printf("MASTER_INITIAL_IMAGE: \n");

        for (int a = 0; a < imgRows; a++) {
            for (int b = 0; b < imgCols; b++) {
                A[a][b] = 0;
                if (debug) printf("%d", A[a][b]);
            }
            if (debug) printf("\n");
        }


        if (debug) printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
               "~~~~~~~~~~~~~~~~ MASTER INITIALIZING ~~~~~~~~~~~~~~~~~~~~\n"
               "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

        clock_t begin = clock();

        for (int i = 1; i < np && totalPartitionsScattered < totalPartitions; i++) {

            if (debug) printf("MASTER_preparing_job: Slave_%d\n", i);
            int x0 = tasks[totalPartitionsScattered][0];
            int y0 = tasks[totalPartitionsScattered][1];
            int x1 = tasks[totalPartitionsScattered][2];
            int y1 = tasks[totalPartitionsScattered][3];

            int tsk[4];
            tsk[0] = x0;
            tsk[1] = y0;
            tsk[2] = x1;
            tsk[3] = y1;

            MPI_Send(&tsk, 4, MPI_INT, i, 1, MPI_COMM_WORLD);
            if (debug) printf("MASTER_sent_TASK: Slave_%d Task: {%d,%d,%d,%d}\n", i, tsk[0], tsk[1], tsk[2], tsk[3]);

            totalPartitionsScattered++;
        }



        if (debug) printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
               "~~~~~~~~~~~~~~~~ MASTER STARTED TO RECV LOOP ~~~~~~~~~~~~~~~~~\n"
               "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        while (totalPartitionsGathered < totalPartitions) {

            int worker;
            int tsk[4];
            MPI_Recv(&tsk, 4, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);

            worker = status.MPI_SOURCE;
            int **B;
            B = alloc_2d_int(imgRowsStep, imgColsStep);

            int x0 = tsk[0];
            int y0 = tsk[1];
            int x1 = tsk[2];
            int y1 = tsk[3];

            int area = (x1 - x0) * (y1 - y0);
            if (debug) printf("MASTER_received_task: Slave_%d Area: %d Task: {%d,%d,%d,%d}\n", status.MPI_SOURCE, area, tsk[0], tsk[1], tsk[2], tsk[3]);


            MPI_Recv(&(B[0][0]), area, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD, &status);

            if (debug) printf("MASTER_received_job: Slave_%d Area: %d Task: {%d,%d,%d,%d}\n", status.MPI_SOURCE, area, tsk[0], tsk[1], tsk[2], tsk[3]);


            string masterImage = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nMASTER_RECEIVED_PART\n";
            for (int a = 0; a < imgRowsStep; a++) {
                for (int b = 0; b < imgColsStep; b++) {
                    char buffer[2];
                    sprintf(buffer, "%d", B[a][b]);
                    masterImage += buffer;
                }
                masterImage += "\n";
            }
            if (debug) cout << masterImage;


            string masterImage3 = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nMASTER_IMAGE_BEFORE\n";
            for (int a = 0; a < imgRows; a++) {
                for (int b = 0; b < imgCols; b++) {
                    char buffer[2];
                    sprintf(buffer, "%d", A[a][b]);
                    masterImage3 += buffer;
                }
                masterImage3 += "\n";
            }
            if (debug) cout << masterImage3;
            
            // PUT MATRIX
            for (int row = y0, a = 0; row < y1 && a < imgRowsStep; row++, a++) {
                for (int col = x0, b = 0; col < x1 && b < imgColsStep; col++, b++) {
                    A[row][col] = B[a][b];
//                    printf("Task: {%d,%d,%d,%d}; %d, %d\n", tsk[0], tsk[1], tsk[2], tsk[3], row, column);
                }
            }
            if (debug) printf("MATRIX PUT SUCCESSFULLY");

            string masterImage2 = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nMASTER_IMAGE_AFTER\n";
            for (int a = 0; a < imgRows; a++) {
                for (int b = 0; b < imgCols; b++) {
                    char buffer[2];
                    sprintf(buffer, "%d", A[a][b]);
                    masterImage2 += buffer;
                }
                masterImage2 += "\n";
            }
            if (gui) cout << masterImage2;

            // PUT MATRIX END


            totalPartitionsGathered++;

            if (totalPartitionsScattered < totalPartitions) {
                int x0 = tasks[totalPartitionsScattered][0];
                int y0 = tasks[totalPartitionsScattered][1];
                int x1 = tasks[totalPartitionsScattered][2];
                int y1 = tasks[totalPartitionsScattered][3];

                int tsk[4];
                tsk[0] = x0;
                tsk[1] = y0;
                tsk[2] = x1;
                tsk[3] = y1;

                MPI_Send(&tsk, 4, MPI_INT, worker, 1, MPI_COMM_WORLD);
                if (debug) printf("MASTER_preparing_ADDITIONAL_job: Slave_%d: {%d,%d,%d,%d}\n", worker, tsk[0], tsk[1], tsk[2], tsk[3]);

                totalPartitionsScattered++;
            }

        }


        if (debug) printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
               "~~~~~~~~~~~~~~~~~~~~~~~~ MASTER EXIT ~~~~~~~~~~~~~~~~~~~~~~~~~\n"
               "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

        clock_t end = clock();

        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        printf("Time: %f\n", elapsed_secs);


        MPI_Abort(MPI_COMM_WORLD, 1);

    } else if (rank != 0) {
        while(1) {
            int **B;
            int tsk[4];

            MPI_Recv(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            if (debug) printf("SLAVE_%d_received_TASK: {%d,%d,%d,%d}\n", rank, tsk[0], tsk[1], tsk[2], tsk[3]);

            int x0 = tsk[0];
            int y0 = tsk[1];
            int x1 = tsk[2];
            int y1 = tsk[3];

            B = alloc_2d_int(imgRowsStep,imgColsStep);



//            string slaveJob1 = "";
            for (int row = y0, a = 0; row < y1; row++, a++) {
                for (int col = x0, b = 0; col < x1; col++, b++) {

//                    B[a][b] = rank;
                    std::complex<float> z, c = {
//                            (float)column * 2 / y1 - 1.5f,
//                            (float)row * 2 / x1 - 1
                            ((float)col - imgCols/2) / (imgCols/4),
                            ((float)row - imgRows/2) / (imgRows/2)
                    };
                    int iteration = 0;
                    while(abs(z) < 2 && ++iteration < maxIter)
                        z = pow(z, 2) + c;
//                    slaveJob1 +=  (iteration == maxIter ? '#' : '.');
                    B[a][b] = (iteration == maxIter ? 1 : 0);
                }
//                slaveJob1 += "\n";
            }

//            cout << slaveJob1;


            MPI_Send(&tsk, 4, MPI_INT, 0, 1, MPI_COMM_WORLD);

            if (debug) printf("##############################################\n"
                    "Slave sent its task back\n"
                    "##############################################\n");

            MPI_Send(&(B[0][0]), imgRowsStep*imgColsStep, MPI_INT, 0, 2, MPI_COMM_WORLD);

            if (debug) printf("SLAVE_%d_submitted_job: {%d,%d,%d,%d}\n", rank, tsk[0], tsk[1], tsk[2], tsk[3]);
        }

    }

    MPI_Finalize();
    return 0;
}
