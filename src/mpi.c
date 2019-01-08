#include "headers.h"
#include <unistd.h>

//#include "../src/multithreading/multithreading.h"
#include <pthread.h>

MPI_MCTS_Result runMPI_MCTS(int numprocs, int rank, char* processor_name, node board, int player, int masterPlayer, int searchDepth, float C, float maxTime);
int runMPISimulation(int numprocs, int rank, char* processor_name, int namelen, node board, int player, int masterPlayer, int searchDepth, float maxTime);

void* cpuThread(void* t);

void* cpuThread(void* t) {
    
	threadData* thread = (threadData*)t;
	timeval current;
	thread->threadResult = NULL;
	
	//printCurrentLocalTime(&(thread->startTime));
	//printf("Machine: %s [%d], Thread %d starting\n",  thread->issuerName, thread->issuerRank, thread->id);
    
	thread->threadResult = MCTS_test (thread->board, thread->player, thread->masterPlayer, thread->searchDepth, thread->timeLimit, thread->C, (thread->startTime), thread->issuerRank, thread->maxprocs);
	
    //	gettimeofday(&current, 0);
	//printCurrentLocalTime(&current);
	
	//if (thread->threadResult != NULL) printf("Machine: %s [%d], thread %d thread->threadResult ok\n", thread->issuerName, thread->issuerRank, thread->id);
	//else  printf("Machine: %s [%d], thread %d thread->threadResult NULL\n",  thread->issuerName, thread->issuerRank, thread->id);
	
    //	gettimeofday(&current, 0);
	//printCurrentLocalTime(&current);
	//printf("Machine: %s [%d], Thread %d ending\n",  thread->issuerName, thread->issuerRank, thread->issuerRank, thread->id);
	pthread_exit(NULL);
    
}

MPI_BOARD makeMpiMove (node board, short player, short masterPlayer, cmdArguments args, int rank, int numprocs, char* processor_name, int maxprocs, int method, short cpus) {
    
    
    int            	packsize, position;
    char         	    packbuf[MPI_BUF_SIZE];
    struct timeval 	startTime, current, interval;
    long				result = 0;
    long				totalSimulations = 0;
    threadData 		cpuThreadData[MAX_THREADS];
    pthread_t 			cpuThreads[MAX_THREADS];
    double				timeElapsed = 0.0f;
    threadData			thread;
    long				totalRootSim;
    long				totalRootSuccess;
    long				*totalSuccessChildren;
    long				*totalSimulationsChildren;
    long				votes[MAX_TASKS_STATIC];
    long				totalvotes[MAX_TASKS_STATIC];
    long				bytesTransferred = 0;
    double				communicationTime = 0.0f;
    MCTSnode* 			root = NULL;
    MPI_MCTS_Result	res;
    int 				searchDepth = args.depth;
    float 				C = args.C;
    float 				timeLimit = args.maxTime;
    char				shortProcessorName[MAX_ISSUER_NAME];
    MPI_BOARD			out;
    int				mpiErr;
    int				gpus = args.gpuThreads;
    
    totalSimulationsChildren = (long *)malloc( 128 * sizeof(long) );
    totalSuccessChildren = (long *)malloc( 128 * sizeof(long) );
    
    gettimeofday(&startTime, 0);
    
    totalRootSuccess = 0;
    totalRootSim = 0;
    
    for (int i = 0; i < MAX_TASKS_STATIC; i++) {
		
		totalSuccessChildren[i] = 0;
		totalSimulationsChildren[i] = 0;
		votes[i] = 0;
		totalvotes[i] = 0;
    }
    
	strncpy(shortProcessorName, processor_name, 15);
   	
    
	//printf("[Process %3d/%3d @ |%32s| ->    MPI_PreInit\t\t] Time: %s\n", rank+1, numprocs, shortProcessorName, getCurrentLocalTime(&startTime));
	
   	//if (rank == 0)
    //printf("[Process %3d/%3d @ |%32s| ->    MPI_PreInit\t\t] Set Iterations: %d, Depth: %d\n", rank+1, numprocs, shortProcessorName, args.benchmarkIterations, args.depth);
    
    gettimeofday(&current, 0);
    
	//printf("[Process %3d/%3d @ |%32s| ->    MPI_Init\t\t] Time passed: %.3f\n",rank+1, numprocs, shortProcessorName, getTimeDiff(&startTime, &current));
    
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
        thread.C = C;
        thread.maxprocs = maxprocs;
        thread.method = method;
        
        MPI_Pack( &thread.id, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize,  MPI_COMM_WORLD );
        MPI_Pack( thread.issuerName, 16, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( thread.board.blackOrWhite, 8, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( thread.board.occupied, 8, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.player, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.masterPlayer, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.startTime.tv_sec, 1, MPI_LONG, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.startTime.tv_usec, 1, MPI_LONG, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.timeLimit, 1, MPI_FLOAT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.C, 1, MPI_FLOAT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.quit, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.searchDepth, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.maxprocs, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        MPI_Pack( &thread.method, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
        
        
		
    }
    
    gettimeofday(&current, 0);
    timeElapsed = getTimeDiff(&startTime, &current);
    
    MPI_Bcast( &packsize, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( packbuf, packsize, MPI_PACKED, 0, MPI_COMM_WORLD );
    
    gettimeofday(&interval, 0);
    communicationTime += getTimeDiff(&current, &interval);
    bytesTransferred += packsize;
    
    
    if (rank != 0) {
		
        
        position = 0;
        
        //check time info, if no time then break
        MPI_Unpack( packbuf, packsize, &position, &thread.id, 1, MPI_SHORT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, thread.issuerName, 16, MPI_CHAR, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, thread.board.blackOrWhite, 8, MPI_CHAR, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, thread.board.occupied, 8, MPI_CHAR, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.player, 1, MPI_SHORT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.masterPlayer, 1, MPI_SHORT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.startTime.tv_sec, 1, MPI_LONG, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.startTime.tv_usec, 1, MPI_LONG, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.timeLimit, 1, MPI_FLOAT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.C, 1, MPI_FLOAT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.quit, 1, MPI_SHORT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.searchDepth, 1, MPI_SHORT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.maxprocs, 1, MPI_SHORT, MPI_COMM_WORLD );
        MPI_Unpack( packbuf, packsize, &position, &thread.method, 1, MPI_SHORT, MPI_COMM_WORLD );
        
    }
    
    printBoard(thread.board);
    
    cpus--;
    pthread_attr_t attr;
    pthread_attr_init(&attr );
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
	//	pthread_mutex_init (&mutexsum, NULL);
    int rc;
    
    for (int i = 0; i < cpus; i++) {
        
        cpuThreadData[i].id = i;
        strncpy(cpuThreadData[i].issuerName, shortProcessorName, 14);
        cpuThreadData[i].issuerName[15] = '\0';
        cpuThreadData[i].player = thread.player;
        cpuThreadData[i].board = thread.board;
        cpuThreadData[i].timeLimit = timeLimit;
        cpuThreadData[i].C = thread.C;
        cpuThreadData[i].quit = thread.quit;
        cpuThreadData[i].startTime = thread.startTime;
        cpuThreadData[i].searchDepth = thread.searchDepth;
        cpuThreadData[i].maxprocs = thread.maxprocs;
        cpuThreadData[i].masterPlayer = thread.masterPlayer;
        cpuThreadData[i].method = thread.method;
        cpuThreadData[i].issuerRank = rank;
        //cpuThreads[i] = cutStartThread((CUT_THREADROUTINE)cpuThread, &cpuThreadData[i]);
        rc = pthread_create(&cpuThreads[i], &attr, cpuThread, (void *)&cpuThreadData[i]);
        if (rc){
            printf("MACHINE: %s,  thread %d, ERROR; return code from pthread_create() is %d\n", shortProcessorName, i, rc);
            
        }
        // else
        //		 printf("MACHINE: %s,  thread %d, pthread_create() is %d\n", shortProcessorName, i, rc);
        
    }
    //pthread_attr_destroy(&attr );
    
	//printf("Rank: %d a\n",  rank);
    
    
    if (gpus > 0) {
		if (rank == 0)  root = MCTS_GPU(thread.board, thread.player, thread.masterPlayer, thread.searchDepth, thread.timeLimit, thread.C, (thread.startTime), rank, thread.maxprocs, args.blocks,
                                        args.threads, args.device);
		else
		{
            root = MCTS (thread.board, thread.player, thread.masterPlayer, thread.searchDepth, 0, thread.C, (thread.startTime), rank, thread.maxprocs);
            sleep(thread.timeLimit);
		}
    }
    else
		//root = MCTS_GPU (thread.board, thread.player, thread.masterPlayer, thread.searchDepth, thread.timeLimit, thread.C, (thread.startTime), rank, thread.maxprocs, args.blocks, args.threads, args.device);
		root = MCTS (thread.board, thread.player, thread.masterPlayer, thread.searchDepth, thread.timeLimit, thread.C, (thread.startTime), rank, thread.maxprocs);
    //if (cpus > 0) cutWaitForThreads(cpuThreads, cpus);
    //for (int i = 0; i < cpus; i++) {
    // rc = pthread_join(cpuThreads[i], NULL);
	//	printf("MACHINE: %s,  thread %d, join is %d\n", shortProcessorName, i, rc);
    
    //}
    //printf("Rank: %d b\n",  rank);
    
    //root = cpuThreadData[0].threadResult;
    
    /*for (int i = 0; i < cpus; i++) {
     
     if (cpuThreadData[i].threadResult != NULL) 	{
     
     //	printf("MACHINE: %s,  rank: %d thread %d root ok\n", shortProcessorName, rank, i);
     
     printf("MACHINE: %s,  thread %d %ld : %ld\n", shortProcessorName, i, cpuThreadData[i].threadResult->success, cpuThreadData[i].threadResult->simulations);
     
     for (int j = 0; j < cpuThreadData[i].threadResult->childrenNo; j++) {
     
     printf("%d - %d - %ld / %ld\n", i, j, (getChild(cpuThreadData[i].threadResult,j)->success), (getChild(cpuThreadData[i].threadResult,j)->simulations));
     
     }
     
     }
     //else
     //	printf("MACHINE: %s,  rank: %d thread %d root NULL\n", shortProcessorName, rank, i);
     
     }
     */
    
    
    //root = cpuThreadData[0].threadResult;
    
    for (int i = 0; i < cpus; i++) {
		
        if (cpuThreadData[i].threadResult) {
			
            //printf("MACHINE: %s,  thread %d %ld : %ld\n", shortProcessorName, i, cpuThreadData[i].threadResult->success, cpuThreadData[i].threadResult->simulations);
            
            root->success += cpuThreadData[i].threadResult->success;
            root->simulations += cpuThreadData[i].threadResult->simulations;
			
            for (int j = 0; j < root->childrenNo; j++) {
                (getChild(root,j)->success) += (double)(getChild(cpuThreadData[i].threadResult,j)->success);
                (getChild(root,j)->simulations) += (double)(getChild(cpuThreadData[i].threadResult,j)->simulations);
            }
			
            
            
        }
    }
    
    for (int i = 0; i < cpus; i++) {
    	deleteMCTS(cpuThreadData[i].threadResult);
    	//if (cpuThreadData[i].threadResult)	delete(cpuThreadData[i].threadResult);
    }
    
    //for (int j = 0; j < root->childrenNo; j++){
    //	printf("rank: %d, %d - %ld / %ld\n", rank, j, (getChild(cpuThreadData[0].threadResult,j)->success), (getChild(cpuThreadData[0].threadResult,j)->simulations));
    // }
    
    
    //  	printf("Rank: %d c\n",  rank);
    
    
    
    //root = MCTS (thread.board, thread.player, thread.masterPlayer, thread.searchDepth, thread.timeLimit, thread.C, (thread.startTime), rank, thread.maxprocs);
	
	//	root = cpuThreadData[0].threadResult;
	//
	//	printf("Name: %s, Rank: %d d, sim: %ld-%ld\n", processor_name, rank, root->success, root->simulations);
    if (root) {
		totalSimulations = root->simulations;
		result = root->success;
    }
	
    
    //printf("[Process %3d/%3d @ |%32s| ->    MPI_PreFinalize\t]\n", rank+1, numprocs,  processor_name);
    
	//printf("[Process %3d/%3d @ |%32s| ->    MPI_Finalize\t] Time passed: %.3f, Simulations = %ld/%ld = %.2f%%\t\t\tTime: %s\n", rank+1, numprocs,  processor_name, timeElapsed, result, totalSimulations, (float)result/(float)totalSimulations, getCurrentLocalTime(&interval));
	
	//printf("Rank: %d e, totalSim: %ld add: %ld totalRoot: %ld add: %ld \n",  rank, totalSimulations, &totalSimulations, totalRootSim, &totalRootSim);
	
    
	MPI_Barrier(MPI_COMM_WORLD);
	
	mpiErr = MPI_Reduce(&totalSimulations, &totalRootSim, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	//printf("Rank: %d e1, mpierr %d\n", rank, mpiErr);
	
    
    mpiErr = MPI_Reduce(&result, &totalRootSuccess, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	
    //if (rank ==0)	printf("Rank: %d e, totalSim: %ld totalRoot: %ld mpiErr %d\n",  rank, totalSimulations, totalRootSim, mpiErr);
	/*
     if (thread.method == MAJORITY_VOTE && rank < thread.maxprocs) {
     
     double best = 0.0f;
     double avgTemp = 0.0f;
     short index = 0;
     
     for (int i = 0; i < root->childrenNo; i++) {
     
     
     
     avgTemp = (double)(getChild(root,i)->success)/(double)(getChild(root,i)->simulations);
     //printf("[Process %3d/%3d @ |%32s| ->   %d, avg = %.4f\n", rank+1, numprocs,  processor_name, i, avgTemp);
     
     if (avgTemp > best) {
     best = avgTemp;
     index = i;
     }
     
     }
     printf("[Process %3d/%3d @ |%32s| -> best = %d, val = %.4f\n", rank+1, numprocs,  processor_name, index, best);
     
     votes[index] = 1;
     
     }
     */
    //	printf("Rank: %d f\n",  rank);
	
	for (int i = 0; i < root->childrenNo; i++) {
        
		
        
		mpiErr = MPI_Reduce(&(getChild(root,i)->simulations), &totalSimulationsChildren[i], 1, MPI_LONG, MPI_SUM, 0,  MPI_COMM_WORLD);
        
        mpiErr = MPI_Reduce(&(getChild(root,i)->success), &totalSuccessChildren[i],  1, MPI_LONG, MPI_SUM, 0,  MPI_COMM_WORLD);
    	//MPI_Reduce(&votes[i], &totalvotes[i],  1, MPI_LONG, MPI_SUM, 0,  MPI_COMM_WORLD);
    	
	}
	//pthread_mutex_destroy(&mutexsum);
    //	printf("Rank: %d g\n",  rank);
	if (rank == 0) {
		
		res.no = root->childrenNo;
		res.rAverage = (float)root->success/(float)root->simulations;
		res.rSuccess = totalRootSuccess;
		res.rTotal   = totalRootSim;
		
		for (int i = 0; i < root->childrenNo; i++) {
			
			res.success[i] = totalSuccessChildren[i];
			res.simulations[i] = totalSimulationsChildren[i];
			res.average[i] = (float)totalSuccessChildren[i]/(float)totalSimulationsChildren[i];
			res.votes[i] = totalvotes[i];
			
		}
        //		printf("Rank: %d h\n",  rank);
		gettimeofday(&interval, 0);
		timeElapsed = getTimeDiff(&startTime, &interval);
        
		if (rank == 0) printf("[Process %3d/%3d @ |%32s| ->    MPI_Finalize\t] P: %d, Total Success/Total = %ld/%ld, S/sec = %.2f, Total Time = %.3f s\n[Process %3d/%3d @ |%32s| ->    MPI_Finalize\t] Transfer Time = %.5f s, %ld B * %d procs = %ld B (%.2f MB/s)\tTime: %s\n",  rank+1, numprocs,  processor_name, thread.player, totalRootSuccess, totalRootSim, (float)totalRootSim/(float)timeElapsed, timeElapsed,   rank+1, numprocs,  processor_name, communicationTime, bytesTransferred, numprocs, bytesTransferred* numprocs, (float)(bytesTransferred* numprocs)/(float)(communicationTime*1024*1024), getCurrentLocalTime(&interval));
        
   	}
    
    if (root) {
        
    	deleteMCTS(root);
    	delete(root);
    }
    
	if (rank == 0) {
        
        
        //printf("%s\n", printMCTSResult(&res));
        
        
        tasks t = getChildren (player, board);
        
        //printf("Rank: %d j\n",  rank);
        if (t.howMany > 0) {
            
            short best = 0;
            float val = -INFINITY;
            
            
			for (int i = 0; i < res.no; i++) {
                
                
                if (thread.method == MAJORITY_VOTE) {
                    
                    //printf("threadp: %d, [%d] vote = %ld, avg = %.2f\n", thread.player, i, res.votes[i], res.average[i]);
                    if (res.votes[i] > val) {
                        val = res.votes[i];
                        best = i;
                    }
                }
                else
                    if (thread.method == AVERAGE_VOTE)
                        if (res.average[i] > val) {
                            val = res.average[i];
                            best = i;
                        }
                
                
            }
            
            board = t.taskList[best];
        }
	} else board = thread.board;
    //	printf("Rank: %d k\n", rank);
	gettimeofday(&current, 0);
    
	//printf("[Process %3d/%3d @ |%32s| ->    MPI_PostFinalize\t] Time: %s\n", rank+1, numprocs, shortProcessorName, getCurrentLocalTime(&current));
    
    
	out.simulationsCPU = totalRootSim;
	out.board = board;
    
	free(totalSuccessChildren);
	free(totalSimulationsChildren);
	
	//printf("Rank: %d out\n",  rank);
	return out;
}

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
   	
   	strncpy(shortProcessorName, processor_name, 14);
   	
    
	printf("[Process %3d/%3d @ |%32s| ->    MPI_PreInit\t\t] Time: %s\n", rank+1, numprocs, shortProcessorName, getCurrentLocalTime(&startTime));
	
   	
   	if (rank == 0)
        printf("[Process %3d/%3d @ |%32s| ->    MPI_PreInit\t\t] Set Iterations: %d, Depth: %d\n", rank+1, numprocs, shortProcessorName, args.benchmarkIterations, args.depth);
    
    gettimeofday(&current, 0);
    
	printf("[Process %3d/%3d @ |%32s| ->    MPI_Init\t\t] Time passed: %.3f\n",rank+1, numprocs, shortProcessorName, getTimeDiff(&startTime, &current));
    
	board = initBoard();
	player = START_PLAYER;
	masterPlayer = START_PLAYER;
	
    
	//running MPI
	for (i = 0; i < args.benchmarkIterations; i++)
        runMPISimulation(numprocs, rank, shortProcessorName, namelen, board, player, masterPlayer, args.depth, args.maxTime);
	
	MPI_Finalize( );
	
	gettimeofday(&current, 0);
    
	printf("[Process %3d/%3d @ |%32s| ->    MPI_PostFinalize\t] Time: %s\n", rank+1, numprocs, shortProcessorName, getCurrentLocalTime(&current));
    
}

int runMPISimulation(int numprocs, int rank, char* processor_name, int namelen, node board, int player, int masterPlayer, int searchDepth, float maxTime) {
    
    int            	packsize, position;
    char         	    packbuf[MPI_BUF_SIZE];
    struct timeval 	startTime, current, interval;
    long				result, totalSimulations;
    double				timeElapsed = 0.0f;
    threadData			thread;
    long				totalRootSim = 0;
    long				totalRootSuccess = 0;
    long				bytesTransferred = 0;
    double				communicationTime = 0.0f;
    
    gettimeofday(&startTime, 0);
    
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
        thread.timeLimit = maxTime;
        thread.quit = 0;
        thread.searchDepth = searchDepth;
        
        MPI_Pack( &thread.id, 1, MPI_SHORT, packbuf, MPI_BUF_SIZE, &packsize,  MPI_COMM_WORLD );
        MPI_Pack( thread.issuerName, 16, MPI_CHAR, packbuf, MPI_BUF_SIZE, &packsize, MPI_COMM_WORLD );
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
        
		printf("[Process %3d/%3d @ |%32s| ->    MPI_Finalize\t] Time passed: %.3f, Simulations = %ld/%ld = %.2f%%\t\t\tTime: %s\n", rank+1, numprocs,  processor_name, timeElapsed, result, totalSimulations, (float)result/(float)totalSimulations, getCurrentLocalTime(&interval));
    
	MPI_Reduce(&totalSimulations, &totalRootSim, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);
    MPI_Reduce(&result, &totalRootSuccess, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);
	
	if (rank == 0) {
		
		gettimeofday(&interval, 0);
		timeElapsed = getTimeDiff(&startTime, &interval);
        
		printf("[Process %3d/%3d @ |%32s| ->    MPI_Finalize\t] Total Success/Total = %ld/%ld, S/sec = %.2f, Total Time = %.3f s\n[Process %3d/%3d @ |%32s| ->    MPI_Finalize\t] Transfer Time = %.5f s, %ld B * %d procs = %ld B (%.2f MB/s)\tTime: %s\n",  rank+1, numprocs,  processor_name, totalRootSuccess, totalRootSim, (float)totalRootSim/(float)timeElapsed, timeElapsed,   rank+1, numprocs,  processor_name, communicationTime, bytesTransferred, numprocs, bytesTransferred* numprocs, (float)(bytesTransferred* numprocs)/(float)(communicationTime*1024*1024), getCurrentLocalTime(&interval));
        
   	}
    
    
    return 0;
}
