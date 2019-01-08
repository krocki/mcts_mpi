#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>
#include "reversi.h"


#define MODE_BENCHMARK 1
#define DEFAULT_BENCHMARK_ITERATIONS 1
#define DEFAULT_BENCHMARK_DEPTH 60
#define DEBUG 0
#define MAX_TASKS_STATIC 16
#define WHITE 1
#define BLACK 0
#define START_PLAYER WHITE
#define MPI_BUF_SIZE 256
#define MAX_ISSUER_NAME 64

#define swapint(x,y) { x ^= y; y ^= x; x ^= y;}
#define negative(x) (~x + 1)
#define mod64(x) (x & 63)
#define mod32(x) (x & 31)
#define mod16(x) (x & 15)
#define mod8(x) (x & 7)
#define mod4(x) (x & 3)
#define mod2(x) (x & 1)

//colors
#define RED 31
#define BLACK_COLOR 30
#define GREEN 32
#define BLUE 34
#define WHITE_COLOR 37
#define MAGENTA 35
#define BRIGHT 1

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

#ifndef _STRUCTS
#define _STRUCTS
	typedef struct  {
	
		node taskList[MAX_TASKS_STATIC];
		char howMany;
	
	} tasks;
	
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
		
	} threadData;

	typedef struct {
	
		int					benchmarkIterations;
		int					benchmarkDepth;
		
	} cmdArguments;
	
	
#endif
////////////FUNCTIONS
extern void simulationsPerSecondBenchmark(cmdArguments args, int argc, const char **argv);
extern int runMPISimulation(int numprocs, int rank, char* processor_name, int namelen, node board, int player, int masterPlayer, int searchDepth);
extern tasks getChildren (int who, node board);
extern node flip (node board, int dirs, int x, int y, int player);
extern int move (node board, int x, int y, int player);
extern short scoreCPU (node board, int player);
extern node makeRandomMove (node board, short who);
extern int performRandomSimulation (node startBoard, short who, short maxDepth, int master_player);
extern node initBoard (void);
extern void printBoard (node board);
extern float getTimeDiff(struct timeval *s, struct timeval *e);
extern void printTypeData(void);
extern void initRand (void);
extern int checkEnd(node board);
extern void printBits(char byte, char size);
extern cmdArguments parseInput(int argc, const char **argv);
