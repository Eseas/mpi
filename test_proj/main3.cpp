/*
 * Copyright 1999 Sun Microsystems, Inc.  901 San Antonio Road, Palo
 * Alto, CA 94303, U.S.A.  All Rights Reserved.
 *
 * The contents of this file contain proprietary material of Sun
 * Microsystems, Inc., and are subject to the current version of the Sun
 * Community Source License for Sun HPC ClusterTools (TM) ("the License").
 * You may not use this file except in compliance with the License.  You
 * may obtain a copy of the License on the World Wide Web from
 * http://www.sun.com/software/communitysource/.  See the License for the
 * rights, obligations, and limitations governing use of the contents of
 * this file.
 *
 * Sun, Sun Microsystems, the Sun logo, Sun HPC ClusterTools, Sun PFS,
 * Sun C++, Sun MPI, Prism, Sun Prism, and all Sun-based trademarks and logos,
 * are trademarks or registered trademarks of Sun Microsystems, Inc. in
 * the United States and other countries.
 */

/*
 * Test the connectivity between all processes.
 */

#pragma ident "@(#)connectivity.c 1.2 99/09/01"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include <mpi.h>

int
main(int argc, char **argv)
{
    MPI_Status	status;
    int		verbose = 0;
    int		rank;
    int		np;				/* number of processes in job */
    int		peer;
    int		i;
    int		j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (argc>1 && strcmp(argv[1], "-v")==0)
        verbose = 1;

    for (i=0; i<np; i++) {
        if (rank==i) {
            /* rank i sends to and receives from each higher rank */
            for(j=i+1; j<np; j++) {
                if (verbose)
                    printf("checking connection %4d <-> %-4d\n", i, j);
                MPI_Send(&rank, 1, MPI_INT, j, rank, MPI_COMM_WORLD);
                MPI_Recv(&peer, 1, MPI_INT, j, j, MPI_COMM_WORLD, &status);
            }
        } else if (rank>i) {
            /* receive from and reply to rank i */
            MPI_Recv(&peer, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
            MPI_Send(&rank, 1, MPI_INT, i, rank, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    /*if (rank==0)*/
    printf("Connectivity test on %d processes PASSED.\n", np);

    MPI_Finalize();
    return 0;
}