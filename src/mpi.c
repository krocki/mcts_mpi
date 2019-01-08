#include "headers.h"

void simulationsPerSecondBenchmark(cmdArguments args, int argc, const char **argv){


	int            		rank;
    int 				numprocs, namelen;
    char*				processor_name;
    struct timeval 		startTime, current;
    int					i;
    node				board;
    int 				player, masterPlayer;
    char				shortProcessorName[MAX_ISSUER_NAME];
    
    //time = 0;
    gettimeofday(&startTime, 0);
    
	MPI_Init( &argc, (char***)&argv );
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 	processor_name = (char *) malloc (MPI_MAX_PROCESSOR_NAME * sizeof (char));
    MPI_Get_processor_name(processor_name, &namelen);
   	
   	initRand ();
   	
   	strncpy(shortProcessorName, processor_name, 24);
   	
   	if (rank == 0) 
   		printf("[Process %3d/%3d @ |%.18s...............\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\t\t| -> \tSet Iterations: %d, Depth: %d\n", rank+1, numprocs, shortProcessorName, args.benchmarkIterations, args.benchmarkDepth);	
   	
    gettimeofday(&current, 0);
	printf("[Process %3d/%3d @ |%.18s...............\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\t\t| -> \tMPI_Init\t] Time passed: %.3f\n",rank+1, numprocs, shortProcessorName, getTimeDiff(&startTime, &current));
	
	board = initBoard();
	player = START_PLAYER;
	masterPlayer = START_PLAYER;
	//running MPI
	for (i = 0; i < args.benchmarkIterations; i++) {
	runMPISimulation(numprocs, rank, shortProcessorName, namelen, board, player, masterPlayer, args.benchmarkDepth);
  printBoard(board);
  }
	
	MPI_Finalize( );
	
}

int runMPISimulation(int numprocs, int rank, char* processor_name, int namelen, node board, int player, int masterPlayer, int searchDepth) {

     int            	packsize, position;
     char         	    packbuf[MPI_BUF_SIZE];
 	 struct timeval 	startTime, current, interval;
	 float				timeLimit = 1;
	 long				result, totalSimulations;
	 int 				time;
	 double				timeElapsed = 0.0f;
	 threadData			thread;
	 long				totalRootSim = 0;
	 long				totalRootSuccess = 0;
	 long				bytesTransferred = 0;
	 double				communicationTime = 0.0f;
		 
	 gettimeofday(&startTime, 0);
	 
     time = 1;
     totalSimulations = 0;
     result = 0;
     thread.quit = 0;
     
     	
		if (rank == 0) {
		
			
		
			packsize = 0;
			
			thread.id = 0;
     		strncpy(thread.issuerName, processor_name, MAX_ISSUER_NAME-1);
     		thread.board = board;
     		thread.player = player;
     		thread.masterPlayer = masterPlayer;
     		thread.startTime.tv_usec = startTime.tv_usec;
     		thread.startTime.tv_sec = startTime.tv_sec;
     		thread.timeLimit = timeLimit;
			thread.quit = 0;
			thread.searchDepth = searchDepth;
			
			
			if (DEBUG) {
     
				 printf( "[BROADCASTER]Process %d on %s: thread.id = %d\n", rank, processor_name, thread.id);
				 printf( "[BROADCASTER]Process %d on %s: thread.issuerName = %s\n", rank, processor_name, thread.issuerName);
				 printf( "[BROADCASTER]Process %d on %s: thread.board = \n", rank, processor_name);
				 printBoard(board);
				 printf( "[BROADCASTER]Process %d on %s: thread.player = %d\n", rank, processor_name, thread.player);
				 printf( "[BROADCASTER]Process %d on %s: thread.masterPlayer = %d\n", rank, processor_name, thread.masterPlayer);
				 printf( "[BROADCASTER]Process %d on %s: thread.startTime = %d.%d\n", rank, processor_name, (unsigned int)thread.startTime.tv_sec, (unsigned int)thread.startTime.tv_usec);
				 printf( "[BROADCASTER]Process %d on %s: thread.timeLimit = %.2f\n", rank, processor_name, thread.timeLimit);    

   		    }
			
		
			MPI_Pack( &thread.id, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize,  MPI_COMM_WORLD );
			MPI_Pack( thread.issuerName, 64, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( thread.board.blackOrWhite, 8, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( thread.board.occupied, 8, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.player, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.masterPlayer, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.startTime.tv_sec, 1, MPI_FLOAT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.startTime.tv_usec, 1, MPI_FLOAT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.timeLimit, 1, MPI_FLOAT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.quit, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			MPI_Pack( &thread.searchDepth, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
			
		
		}

		gettimeofday(&current, 0);
		
		MPI_Bcast( &packsize, 1, MPI_INT, 0, MPI_COMM_WORLD );
		MPI_Bcast( packbuf, packsize, MPI_PACKED, 0, MPI_COMM_WORLD );
		
		gettimeofday(&interval, 0);
			communicationTime += getTimeDiff(&current, &interval);
			bytesTransferred += packsize;
			
		if (rank != 0) {
		
				
			position = 0;
			
			//check time info, if no time then break
			MPI_Unpack( packbuf, packsize, &position, &thread.id, 1, MPI_SHORT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, thread.issuerName, MAX_ISSUER_NAME, MPI_CHAR, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, thread.board.blackOrWhite, 8, MPI_CHAR, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, thread.board.occupied, 8, MPI_CHAR, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.player, 1, MPI_SHORT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.masterPlayer, 1, MPI_SHORT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.startTime.tv_sec, 1, MPI_FLOAT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.startTime.tv_usec, 1, MPI_FLOAT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.timeLimit, 1, MPI_FLOAT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.quit, 1, MPI_SHORT, MPI_COMM_WORLD );
			MPI_Unpack( packbuf, packsize, &position, &thread.searchDepth, 1, MPI_SHORT, MPI_COMM_WORLD );
		}
		
		if (DEBUG) {
		
				 printf( "[RECEIVER]Process %d on %s: thread.id = %d\n", rank, processor_name, thread.id);
				 printf( "[RECEIVER]Process %d on %s: thread.issuerName = %s\n", rank, processor_name, thread.issuerName);
				 printf( "[RECEIVER]Process %d on %s: thread.board = \n", rank, processor_name);
				 printBoard(board);
				 printf( "[RECEIVER]Process %d on %s: thread.player = %d\n", rank, processor_name, thread.player);
				 printf( "[RECEIVER]Process %d on %s: thread.masterPlayer = %d\n", rank, processor_name, thread.masterPlayer);
				 printf( "[RECEIVER]Process %d on %s: thread.startTime = %d.%d\n", rank, processor_name, (unsigned int)thread.startTime.tv_sec, (unsigned int)thread.startTime.tv_usec);
				 printf( "[RECEIVER]Process %d on %s: thread.timeLimit = %.2f\n", rank, processor_name, thread.timeLimit);
				 gettimeofday(&interval, 0);
				 timeElapsed = getTimeDiff(&current, &interval);
				 printf( "[RECEIVER]Process %d on %s: time elapsed = %.5f\n", rank, processor_name, timeElapsed);
			}
		
		if (rank != 0)
		
			while (thread.quit != 1) {
			
				totalSimulations++;
				result += performRandomSimulation (thread.board, thread.player, thread.searchDepth, thread.masterPlayer);
				
				gettimeofday(&interval, 0);
				timeElapsed = getTimeDiff(&startTime, &interval);
				
				if (timeElapsed >= thread.timeLimit)
				thread.quit = 1;
				
			}
		
	if (rank != 0)	
	printf("[Process %3d/%3d @ |%.18s...............\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\t\t| -> \tMPI_Finalize\t] Time passed: %.3f, Simulations = %ld/%ld = %.2f%%\n", rank+1, numprocs,  processor_name, timeElapsed, result, totalSimulations, (float)result/(float)totalSimulations);
	
	MPI_Reduce(&totalSimulations, &totalRootSim, 1, MPI_INT, MPI_SUM, 0, 
                MPI_COMM_WORLD);
    MPI_Reduce(&result, &totalRootSuccess, 1, MPI_INT, MPI_SUM, 0, 
                MPI_COMM_WORLD);            
	
	if (rank == 0)
	printf("[Process %3d/%3d @ |%.18s...............\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\t\t| -> \tMPI_Finalize\t] Total Success/Total = %ld/%ld, S/sec = %.2f, Transfer Time = %.5f s, %ld B * %d procs = %ld B (%.2f MB/s)\n",  rank+1, numprocs,  processor_name, totalRootSuccess, totalRootSim, (float)totalRootSim/(float)timeLimit, communicationTime, bytesTransferred, numprocs, bytesTransferred* numprocs, (float)(bytesTransferred* numprocs)/(float)(communicationTime*1024*1024));
   
    
   
    return 0;
}
