#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <mpi.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include "reversi.h"
#include <sys/select.h>

#define MODE_BENCHMARK 0
#define MODE_MCTS_TEST 0
#define MODE_MPI_MCTS 1
#define MODE_GAME_SIMULATIONS 0

#define DEFAULT_BENCHMARK_ITERATIONS 1
#define DEFAULT_DEPTH 60
#define DEFAULT_TIME 100.0f
#define DEFAULT_C 1.0
#define DEBUG 0
#define MAX_TASKS_STATIC 16
#define WHITE 1
#define BLACK 0
#define START_PLAYER WHITE
#define MPI_BUF_SIZE 96
#define MAX_ISSUER_NAME 32
#define MAX_THREADS 64

#define MAJORITY_VOTE 1
#define AVERAGE_VOTE 2

#define MAX_RAND 1024 * 128

#define swapint(x,y) { x ^= y; y ^= x; x ^= y;}
#define negative(x) (~x + 1)
#define mod64(x) (x & 63)
#define mod32(x) (x & 31)
#define mod16(x) (x & 15)
#define mod8(x) (x & 7)
#define mod4(x) (x & 3)
#define mod2(x) (x & 1)

//bits
#define BIT_0 0x01
#define BIT_1 0x02
#define BIT_2 0x04
#define BIT_3 0x08
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80
#define ALL_1 0xFF

#define INIT_CURSES() 			initscr(); 								 \
start_color();							 \
init_pair(1, COLOR_RED, COLOR_WHITE);	 \
init_pair(4, COLOR_RED, COLOR_BLACK);	 \
init_pair(2, COLOR_WHITE, COLOR_BLACK);	 \
init_pair(3, COLOR_GREEN, COLOR_BLACK);

#define END_CURSES()			endwin();

#define CURSES_REPAINT()		printBoard(displayBoard);


#ifndef _STRUCTS
#define _STRUCTS

typedef struct  {
	
    node 				board;
    long				simulationsCPU;
	
}	MPI_BOARD;

typedef struct  {
	
    node 				taskList[MAX_TASKS_STATIC];
    char 				howMany;
	
} tasks;

typedef struct structMCTSnode {
    
    node 					board;
    struct structMCTSnode*			parent;
    long 					simulations;
    long 					success;
    unsigned 				childrenCreated;
    struct structMCTSnode* 		children;
    unsigned short			childrenNo;
    unsigned short 			who;
    unsigned short 			active;
    char 					id[64];
    unsigned short 			depth;
    
} MCTSnode;

typedef struct {
	
    short 				id;
    char 				issuerName[MAX_ISSUER_NAME];
    node 				board;
    short 				player;
    short 				masterPlayer;
    struct timeval 		startTime;
    float 				timeLimit;
    short				quit;
    short				searchDepth;
    float				C;
    short				maxprocs;
    short				method;
    MCTSnode*			threadResult;
    short				issuerRank;
    short				blocks;
    short				threads;
    
} threadData;

typedef struct {
	
    int					benchmarkIterations;
    int					depth;
    double				maxTime;
    float				C;
    int					voteType[2];
    int					maxprocs;
    int					cpuThreads;
    int					gpuThreads;
    short				blocks;
    short				threads;
    
} cmdArguments;


typedef struct {
	
    int no;
    unsigned long rSuccess;
    unsigned long rTotal;
    float rAverage;
    unsigned long success[MAX_TASKS_STATIC];
    unsigned long simulations[MAX_TASKS_STATIC];
    float average[MAX_TASKS_STATIC];
    unsigned long votes[MAX_TASKS_STATIC];
    
} MPI_MCTS_Result;

#endif

#define getChild(node, i) 			((MCTSnode*)(((node)->children)+(i)))



////////////FUNCTIONS
extern MPI_BOARD 		makeMpiMove (node board, short who, short masterPlayer, cmdArguments args, int rank, int numprocs, char* processor_name, int maxprocs, int method, short cpus);
extern MPI_BOARD 		makeMpiMove_thread (node board, short who, short masterPlayer, cmdArguments args, int rank, int numprocs, char* processor_name, int maxprocs, int method, int cpuThreads);
extern void				deleteMCTS (MCTSnode* root);
extern void				runGames(cmdArguments args, int numprocs, int rank, char* processor_name, int namelen);
extern MPI_MCTS_Result	MPI_MCTS(node board, short player, short masterPlayer, cmdArguments args, int rank, int numprocs, char* processor_name);
extern MPI_MCTS_Result	MPI_MCTS_thread(node board, short player, short masterPlayer, cmdArguments args, int rank, int numprocs, char* processor_name);
extern void 			MCTSTest(cmdArguments args, int argc, const char **argv);
extern void 			simulationsPerSecondBenchmark(cmdArguments args, int argc, const char **argv);
extern void 			simulationsPerSecondBenchmark_thread(cmdArguments args, int argc, const char **argv);
extern int 				runMPISimulation(int numprocs, int rank, char* processor_name, int namelen, node board, int player, int masterPlayer, int searchDepth);
extern int 				runMPISimulation_thread(int numprocs, int rank, char* processor_name, int namelen, node board, int player, int masterPlayer, int searchDepth);
extern void 			printMCTSnode (MCTSnode* node, float C, short includeChildren);
extern MCTSnode*		MCTS (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int max);
extern MCTSnode*		MCTS_GPU (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int max, short b, short t );
extern MCTSnode*		MCTS_test (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int max);
extern tasks 			getChildren (int who, node board);
extern node 			flip (node board, int dirs, int x, int y, int player);
extern int 				move (node board, int x, int y, int player);
extern short 			scoreCPU (node board, int player);
extern node 			makeRandomMove (node board, short who);
extern int 				performRandomSimulation (node startBoard, short who, short maxDepth, int master_player);
extern node 			initBoard (void);
extern void 			printBoard (node board);
extern void				printBoardCurses(node board);
extern float 			getTimeDiff(struct timeval *s, struct timeval *e);
extern char* 			getCurrentLocalTime(struct timeval *s);
extern void 			printCurrentLocalTime(struct timeval *s);
extern void 			printTypeData(void);
extern void 			initRand (void);
extern char*			printMCTSResult(MPI_MCTS_Result*);
extern int 				checkEnd(node board);
extern void 			printBits(char byte, char size);
extern cmdArguments 	parseInput(int argc, const char **argv);

