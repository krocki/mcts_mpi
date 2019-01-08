#include "headers.h"
#include <unistd.h>

int main(int argc, const char** argv ) {
	
	int            				rank;
    int 						numprocs, namelen;
    char*						processor_name;
	cmdArguments 				args = parseInput(argc, argv);
	
	
	if (MODE_BENCHMARK)
        
		simulationsPerSecondBenchmark(args, argc, argv);
    
    
	if (MODE_MPI_MCTS) {
        
		MPI_Init( &argc, (char***)&argv );
		MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		processor_name = (char *) malloc (MPI_MAX_PROCESSOR_NAME * sizeof (char));
		MPI_Get_processor_name(processor_name, &namelen);
		
		if (rank == 0) printTypeData();
		setDevice(args.device);
		node board = initBoard();
		
		runGames(args, numprocs, rank, processor_name, namelen);
		
		MPI_Finalize( );
        
	}
}

void runGames(cmdArguments args, int numprocs, int rank, char* processor_name, int namelen) {
    
    node                                    board;
    short                                   player;
    long                                    iter = args.benchmarkIterations;
    short                                   flip = 0;
    long                                    games = 0;
    long                                    wins = 0;
    long                                    draws = 0;
    long                                    losses = 0;
    long                                    totalScore = 0;
    struct timeval                  startTime, currentTime;
    int                                             step;
    node                                    tempBoard;
    long long                                averageSimulations = 0;
    long long                                   averageSimulations2 = 0;
    float                                   averageScore1 = 0;
    float                                   averageScore2 = 0;
    float                                   averagePerc = 0;
    long long                                   as[64];
    long long                                   as2[64];
    long                                    moves = 0;
    long                                    moves2 = 0;
    MPI_BOARD                               mpi_result;
    double                                  simPerGame;
    double                                  simPerGame2;
    double                                  sim[64];
    double                                  sim2[64];
    short                                   i;
    double                                  s = 0;
    double                                  sw = 0;
    
    double                                  results[10000];
    short                                   resultsw[10000];
    short                                   scores1[10000];
    short                                   scores2[10000];
    float                                   perc[10000];
    gettimeofday(&currentTime, 0);
    long                                    movestotal = 0;
    
	
	for (int i = 0; i < 64; i++) {
        as[i] =0;
        as2[i] = 0;
        
	}
	if (rank == 0) {
		printf("id = %.2d Root start Time: %s\n", rank, getCurrentLocalTime(&currentTime));
		
		
		
	}
	else {
		printf("id = %.2d other start Time: %s\n", rank, getCurrentLocalTime(&currentTime));
		
        
        
        
	}
	
	for (i = 0; i < iter; i++) {
        
        step = 0;
        flip = i % 2;
        if (rank == 0) {
            
            gettimeofday(&startTime, 0);
            
            
            //				flip = i % 2;
            
            
            player = !START_PLAYER;
            board = initBoard();
            initRand ();
        }
        movestotal++;
        while (1) {
			
            //if (rank == 0) {
            
            
            
			
            //		printf("%d %d\n", flip, step);
            if ( step > 0 || flip == 0) {
                
                step++;
                
				//			printf("pl %d max %d vote %d, rank %d\n", player, args.maxprocs, args.voteType[0], rank);
                //	mpi_result = makeMpiMove (board, player, player, args, rank, numprocs, processor_name, args.maxprocs, args.voteType[1], 1);
                
                //	tempBoard = mpi_result.board;
                
                
                //averageSimulations += mpi_result.simulationsCPU;
                //moves2++;
                //simPerGame2 = averageSimulations2/moves2;
                /*	args.gpuThreads = 0;
                 
                 mpi_result = makeMpiMove(board, player, player, args, rank, numprocs, processor_name, args.cpuThreads, args.voteType[1], args.cpuThreads);
                 
                 
                 if (step > 1 & step < 55) {
                 averageSimulations2 += mpi_result.simulationsCPU;
                 moves2++;
                 simPerGame2 = (float)averageSimulations2/(float)moves2;
                 }
                 as2[step+flip] += mpi_result.simulationsCPU;
                 sim2[step+flip] = (float)as2[step+flip]/(float)movestotal;
                 tempBoard = mpi_result.board;
                 //if (rank == 0) {
                 board = tempBoard;
                 */
                board = makeRandomMove (board, player);
                
                
                //
                //	board = tempBoard;
                printBoard(board);
                //	printf("%d SCORE: %d %d\n", player, scoreCPU (board, player), mpi_result.simulationsCPU);
                
                
                
                
				//	}
                
                
                
            }
            
            
            player = !player;
            
            //printf("SIM Per Game : %ld, %ld, %.2f\n", averageSimulations, moves, (float)(simPerGame)/(args.maxTime));
            
            if (step >= 61) break;
            
            args.gpuThreads = 1;
            //printf("pl %d max %d vote %d, rank %d\n", player, args.maxprocs, args.voteType[1], rank);
            //printf("rank %d start\n", rank);
            //if (rank !=0) sleep(args.maxTime*2);
            
            if (rank == 0)
				
                mpi_result = makeMpiMove(board, player, player, args, rank, numprocs, processor_name, 1, args.voteType[1], 1);
            else sleep(	args.maxTime);
            if (rank == 0) {
                tempBoard = mpi_result.board;
                
                if (step > 1 && step < 55) { averageSimulations += mpi_result.simulationsCPU;
                    moves++;							simPerGame = (float)averageSimulations/(float)moves;
                }
				
                as[step+flip] += mpi_result.simulationsCPU;
            }
            sim[step+flip] = (float)as[step+flip]/(float)movestotal;
            //board = makeRandomMove (board, player);
            step++;
            
            
            if (rank == 0) {
                //		board = makeRandomMove (board, player);
                board = tempBoard;
                printBoard(board);
                //printf("%d SCORE: %d %d\n", player, scoreCPU (board, player), mpi_result.simulationsCPU);
                
                
                
                
                
                
            }
            //printf("rank %d end\n", rank);
            player = !player;
            //if (rank == 0) printf("SIM Per Game : %ld, %ld, %.2f/s\n", averageSimulations, moves, (float)(simPerGame)/(args.maxTime));
            
            if (step >= 61) break;
        }
        
        
        if (rank == 0) {
            
            games++;
            
            if (scoreCPU (board, !START_PLAYER) < scoreCPU (board, START_PLAYER)) { wins++; resultsw[games-1] = 1; }
            if (scoreCPU (board, !START_PLAYER) == scoreCPU (board, START_PLAYER)) {draws++; resultsw[games-1] = 0; }
            if (scoreCPU (board, !START_PLAYER) > scoreCPU (board, START_PLAYER)) {losses++; resultsw[games-1] = 0; }
            scores1[games-1] = scoreCPU (board, START_PLAYER);
            scores2[games-1] = scoreCPU (board, !START_PLAYER);
            perc[games-1] = 100*scoreCPU (board, START_PLAYER)/(float)(scoreCPU (board, START_PLAYER)+scoreCPU (board, !START_PLAYER));
            
            totalScore += scoreCPU (board, START_PLAYER) - scoreCPU (board, !START_PLAYER);
            results[games-1] = scoreCPU (board, START_PLAYER) - scoreCPU (board, !START_PLAYER);
            printf("G: %ld, W: %ld, D: %ld L: %ld %.3f\n", games, wins, draws, losses, (float)wins/(float)(wins+losses));
            printf("%ld %d, Average Score: %.3f\n", totalScore, i+1, (float)totalScore/(float)(i+1));
            printf("SIM Per Game : %ld, %ld, %.2f/s\n", averageSimulations, moves, (float)(simPerGame)/(float)(args.maxTime));
            averageScore1 = 0;
            averageScore2 = 0;
            averagePerc = 0;
            
            for (int k  = 0; k < games; k++) {
                
                averageScore1 += scores1[k]/(float)games;
                averageScore2 += scores2[k]/(float)games;
                averagePerc += perc[k]/(float)games;
                
            }
            
            printf("Avg score1: %.2f, avg score2: %.2f, avg perc: %.2f, avg p1 alt: %.1f\n", averageScore1, averageScore2, averagePerc, 64.0f - averageScore2);
            
            
            float avg = (float)totalScore/(float)(i+1);
            s = 0;
            sw = 0;
            if (games > 1) {
                for (long i = 0; i < games; i++) {
					printf("%.2f,", results[i]);
					
                    s += sqrt (
                               
                               ((results[i] - avg )*(results[i] - avg ))
                               );
                    
                    sw += sqrt (
								
								((resultsw[i] - (float)wins/(float)games )*(resultsw[i] - (float)wins/(float)games ))
                                );
                }
                printf("\n");
                float sigma1 = s/(sqrt(games-1));
                float sigma2 = sw/(sqrt(games-1));
                printf("var1: %.2f var2: %.2f, var3: %.2f var4: %.2f\n",sigma1, sigma1/sqrt(games),sigma2, sigma2/sqrt(games));
                //printf("%ld %d, OPP: Average Score: %.3f\n", totalScore, i+1, (float)totalScore/(float)(i+1));
            }
            printf("[OPP]SIM Per Game : %ld, %ld, %.2f/s\n", averageSimulations2, moves, (float)(simPerGame2)/(float)(args.maxTime)); 
            for (int i =0; i<31; i++) {
                
                printf("%d,%.1f,%.1f\n",i, sim[2*i+1]/(float)(args.maxTime), sim2[2*i+1]/(float)(args.maxTime));
                
            }
        }
	}
    //	MPI_Barrier(MPI_COMM_WORLD);
    
    gettimeofday(&currentTime, 0);
    printf("%d end Time: %s\n", rank, getCurrentLocalTime(&currentTime));
    
}
