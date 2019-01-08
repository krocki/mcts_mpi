#ifndef _MCTS_GPU_CU_
#define _MCTS_GPU_CU_

#include "headers_gpu.h"

/*__constant__ short BitsSetTable256[256] =
 
 {
 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
 };*/

#define CHECK_DIR(x,y,who) \
if (!IS_OCCUPIED(board, x, y))\
{\
dirs = moveGPU(board, x, y, who);\
if (IS_SET_DIR_ANY(dirs) && MAX_TASKS_STATIC > t.howMany)\
t.taskList[t.howMany++] = flipGPU(board, dirs, x, y, who);\
}

#define MAX_GPU_THREADS 1024
#define GROUP 32

void checkCUDAError(const char *msg);
__device__ short scoreGPU (node board, int player);
__global__ void MCTS(node* i_value, short *s, short player, short* random);
__host__ __device__ tasks getChildrenGPU (int who, node board);
__device__ __host__ node flipGPU (node board, int dirs, int x, int y, int player);
__host__ __device__ int moveGPU (node board, int x, int y, int player);
__device__ short rG(node startBoard, int startPlayer, short* randoms, short maxDepth, int randOffset, int master_player);
__device__ short rG2(node startBoard, int startPlayer, short* randoms, short maxDepth, int randOffset, int master_player);

__global__ void MCTS(node* i_value, short *s, short player, short* randoms) {
	
	const unsigned long total = gridDim.x * blockDim.x;
	const unsigned long tid = (blockDim.x * blockIdx.x + threadIdx.x);
    
	short startPlayer;//[MAX_GPU_THREADS];
	
	startPlayer = randoms[total+3 ];
	
	int depth = randoms[total+1];
	short master_player = randoms[total+2];
	
	//s[tid] = 0;
	
	s[tid] = rG2(i_value[blockIdx.x * blockDim.x], startPlayer, randoms, 60, 0, master_player);
    
	__syncthreads();
	
}

void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err)
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
        exit(-1);
    }
}

__device__ short scoreGPU (node board, int player) {
	
    short score = 0;
    int x,y;
    /*	for (x = 0; x < 2; x++) {
     score +=  ((BitsSetTable256[board.blackOrWhite[player][x]] -   BitsSetTable256[board.blackOrWhite[!player][x]]) +
     (BitsSetTable256[board.blackOrWhite[player][x+2]] - BitsSetTable256[board.blackOrWhite[!player][x+2]])+
     (BitsSetTable256[board.blackOrWhite[player][x+4]] - BitsSetTable256[board.blackOrWhite[!player][x+4]])+
     (BitsSetTable256[board.blackOrWhite[player][x+6]] - BitsSetTable256[board.blackOrWhite[!player][x+6]]));
     }
     */
	for (x = 0; x < 8; x++)
        for (y = 0; y < 8; y++)
            score += BOARD_GET_SCORE(board, x, y, player) -  BOARD_GET_SCORE(board, x, y, !player);
    
	return score;
	
}
//#define SMEM
__device__ short rG2(node startBoard, int startPlayer, short* randoms, short maxDepth, int randOffset, int master_player) {
	
	
    const unsigned long tid = (blockDim.x * blockIdx.x + threadIdx.x);
    const unsigned long total = gridDim.x * blockDim.x;
    __shared__ short localrands[128];
    
#ifdef SMEM
	//__syncthreads();
    localrands[threadIdx.x] = randoms[tid];
    // __syncthreads();
#endif
    tasks temp;
    int rand;
    int result = 0;
    
    for (int i = 0; i < maxDepth; i++) {
        
        temp = getChildrenGPU(startPlayer, startBoard);
        
        if (temp.howMany > 0)
        {
            //put to shared mem
            //
#ifdef SMEM
            rand = localrands[(threadIdx.x + i) % blockDim.x] % temp.howMany;
#else
            rand = randoms[(tid + i) % total] % temp.howMany;
#endif
            startBoard = temp.taskList[rand];
            startPlayer = !startPlayer;
            __syncthreads();
        }
        
        
        else break;
        __syncthreads();
        
    }
    
	//if (scoreGPU(startBoard,master_player) > 0) result = 1;
    //result = scoreGPU(startBoard,master_player);
	return 64 - scoreGPU(startBoard,!master_player);
}

__device__ short rG(node startBoard, int startPlayer, short* randoms, short maxDepth, int randOffset, int master_player) {
	
	
    const unsigned long tid = (blockDim.x * blockIdx.x + threadIdx.x);
    const unsigned long total = gridDim.x * blockDim.x;
    __shared__ short localrands[128];
    
#ifdef SMEM
	//__syncthreads();
    localrands[threadIdx.x] = randoms[tid];
    // __syncthreads();
#endif
    tasks temp;
    int rand;
    int result = 0;
    
    for (int i = 0; i < maxDepth; i++) {
        
        temp = getChildrenGPU(startPlayer, startBoard);
        
        if (temp.howMany > 0)
        {
            //put to shared mem
            //
#ifdef SMEM
            rand = localrands[(threadIdx.x + i) % blockDim.x] % temp.howMany;
#else
            rand = randoms[(tid + i) % total] % temp.howMany;
#endif
            startBoard = temp.taskList[rand];
            startPlayer = !startPlayer;
            __syncthreads();
        }
        
        
        else break;
        __syncthreads();
        
    }
    
	if (scoreGPU(startBoard,master_player) > 0) result = 1;
    //result = scoreGPU(startBoard,master_player);
	return result;
}


__host__ tasks getChildrenGPU (int who, node board) {
	
	tasks t;
	t.howMany = 0;
	int dirs;
	int x;
    
	
	for (x = 0; x < 8; x++) {
        
        
		CHECK_DIR(x,0,who);
		CHECK_DIR(x,1,who);
        
		CHECK_DIR(x,2,who);
		CHECK_DIR(x,3,who);
        
		CHECK_DIR(x,4,who);
		CHECK_DIR(x,5,who);
        
		CHECK_DIR(x,6,who);
		CHECK_DIR(x,7,who);
        
	}
    
	return t;
    
}


__device__ __host__ node flipGPU (node board, int dirs, int x, int y, int player) {
	
	int i;
	
	BOARD_SET_PLAYER(board, x, y, player);
	
	if ( IS_SET_DIR_DOWN(dirs) )
        for (i = x; !BOARD_IS_SET(board, i + 1, y, player); i++)
            BOARD_FLIP(board, i + 1, y);
	
	if ( IS_SET_DIR_UP(dirs) )
        for (i = x; !BOARD_IS_SET(board, i - 1, y, player); i--)
            BOARD_FLIP(board, i - 1, y);
	
	if ( IS_SET_DIR_LEFT(dirs) )
        for (i = y; !BOARD_IS_SET(board, x, i - 1, player); i--)
            BOARD_FLIP(board, x, i - 1);
	
	if ( IS_SET_DIR_RIGHT(dirs) )
        for (i = y; !BOARD_IS_SET(board, x, i + 1, player); i++)
            BOARD_FLIP(board, x, i + 1);
	
	if ( IS_SET_DIR_DOWN_RIGHT(dirs) )
        for (i = 1; !BOARD_IS_SET(board, x + i, y + i, player); i++)
            BOARD_FLIP(board, x + i, y + i);
	
	if ( IS_SET_DIR_DOWN_LEFT(dirs) )
        for (i = 1; !BOARD_IS_SET(board, x + i, y - i, player); i++)
            BOARD_FLIP(board, x + i, y - i);
	
	if ( IS_SET_DIR_UP_RIGHT(dirs) )
        for (i = 1; !BOARD_IS_SET(board, x - i, y + i, player); i++)
            BOARD_FLIP(board, x - i, y + i);
	
	if ( IS_SET_DIR_UP_LEFT(dirs) )
        for (i = 1; !BOARD_IS_SET(board, x - i, y - i, player); i++)
            BOARD_FLIP(board, x - i, y - i);
    
	return board;
    
}

__host__ __device__ int moveGPU (node board, int x, int y, int player) {
	
	int dirs = 0;
	int i;
	
	//Left
	if (y > 1)
        if (BOARD_IS_SET(board, x, y-1, !player))
        {
            for (i = y; i > 1; i--)
            {
                if (BOARD_IS_SET(board, x,  i - 2, player)
                    && BOARD_IS_SET(board, x,  i - 1, !player))
                {
                    
                    SET_DIR_LEFT(dirs);
                    break;
                }
                else if (!BOARD_IS_SET(board, x,  i - 2, !player)
                         && BOARD_IS_SET(board, x,  i - 1, !player))
                    break;
            }
        }
    
	//Down left
	if (x < 6 && y > 1)
        if BOARD_IS_SET(board, x + 1,  y - 1, !player)
        {
            for (i = 0; ((x + i) < 6 && (y - i) > 2); i++)
            {
                if (BOARD_IS_SET(board, x + i + 2, y - i - 2, player)
                    && BOARD_IS_SET(board, x + i + 1, y - i - 1, !player))
                {
                    SET_DIR_DOWN_LEFT(dirs);
                    break;
                }
                else if (!(BOARD_IS_SET(board, x + i + 2, y - i - 2, !player)
                           && BOARD_IS_SET(board, x + i + 1, y - i - 1, !player)))
                    break;
            }
        }
	
	//down
	if (x < 6)
        if (BOARD_IS_SET(board, x+1, y, !player))
        {
            for (i = x; i < 6; i++)
            {
                if (BOARD_IS_SET(board, i+2, y, player) && BOARD_IS_SET(board, i+1, y, !player)) {
                    SET_DIR_DOWN(dirs);
                    break;
                }
                
                else if (!BOARD_IS_SET(board, i+2, y, !player) && BOARD_IS_SET(board, i+1, y, !player))
                    break;
            }
        }
    
    //Down right
	if (x < 6 && y < 6)
        
        if BOARD_IS_SET(board, x + 1,  y + 1, !player)
        {
            for (i = 0; ((x + i) < 6 && (y + i) < 6); i++)
            {
                if (BOARD_IS_SET(board, x + i + 2, y + i + 2, player)
                    && BOARD_IS_SET(board, x + i + 1, y + i + 1, !player))
                {
                    SET_DIR_DOWN_RIGHT(dirs);
                    break;
                }
                else if (!BOARD_IS_SET(board, x + i + 2, y + i + 2, !player)
                         && BOARD_IS_SET(board, x + i + 1, y + i + 1, !player))
                    break;
            }
        }
    
	//Right
	if (y < 6)
        if (BOARD_IS_SET(board, x,  y + 1, !player))
        {
            for (i = y; i < 6; i++)
            {
                if (BOARD_IS_SET(board, x,  i + 2, player)
                    && BOARD_IS_SET(board, x,  i + 1, !player))
                {
                    SET_DIR_RIGHT(dirs);
                    break;
                }
                else if (!BOARD_IS_SET(board, x,  i + 2, !player)
                         && BOARD_IS_SET(board, x,  i + 1, !player))
                    break;
            }
        }
	
    //Up right
	if (x > 1 && y < 6)
        if BOARD_IS_SET(board, x - 1,  y + 1, !player)
        {
            for (i = 0; ((x - i) > 1 && (y + i) < 6); i++)
            {
                if (BOARD_IS_SET(board, x - i - 2, y + i + 2, player)
                    && BOARD_IS_SET(board, x - i - 1, y + i + 1, !player))
                {
                    SET_DIR_UP_RIGHT(dirs);
                    break;
                }
                else if (!(BOARD_IS_SET(board, x - i - 2, y + i + 2, !player)
                           && BOARD_IS_SET(board, x - i - 1, y + i + 1, !player)))
                    break;
            }
        }
	
	//Up
	if (x > 1)
        if (BOARD_IS_SET(board, x-1, y, !player))
        {
            for (i = x; i > 1; i--)
            {
                if (BOARD_IS_SET(board, i-2, y, player) && BOARD_IS_SET(board, i-1, y, !player)) {
                    SET_DIR_UP(dirs);
                    break;
                }
                else if (!BOARD_IS_SET(board, i-2, y, !player)
                         && BOARD_IS_SET(board, i-1, y, !player))
                    break;
            }
        }
	
	//Up left
	if (x > 1 && y > 1)
        
        if BOARD_IS_SET(board, x - 1,  y - 1, !player)
        {
            for (i = 0; ((x - i) > 1 && (y - i) > 1); i++)
            {
                if (BOARD_IS_SET(board, x - i - 2, y - i - 2, player)
                    && BOARD_IS_SET(board, x - i - 1, y - i - 1, !player))
                {
                    SET_DIR_UP_LEFT(dirs);
                    break;
                }
                else if (!(BOARD_IS_SET(board, x - i - 2, y - i - 2, !player)
                           && BOARD_IS_SET(board, x - i - 1, y - i - 1, !player)))
                    break;
            }
        }
    /*
     */
	return dirs;
	
	
}

#endif
