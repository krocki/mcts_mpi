#include "headers_gpu.h"
#include <cuda_runtime.h>
#include <cuda.h>
#include "mcts_gpu.cu"
/* helpful macro for catching CUDA errors */
#define CUDA_SAFE_CALL(call) do{\
cudaError_t err = call ; \
if (cudaSuccess != err){\
fprintf(stderr, "cuda error at %s:%d, %s\n",\
__FILE__, __LINE__, cudaGetErrorString(err));\
}\
}while(0)


#define getAverage(node) 			((double)node->success)/((double)node->simulations)
#define calculateMCTSVal(node, C) 	(getAverage(node) + sqrt( (C * log(node->parent->simulations))/ log(node->simulations)))


void 			backPropagate(MCTSnode* leaf, long sim, long suc);
void 			initMCTSnode(MCTSnode **node, MCTSnode * parent, short masterPlayer, struct node_struct board, char* chars, short childNum);
MCTSnode* 		selectNode(MCTSnode* root, float C, MCTSnode* best);
long 			nodeCount(MCTSnode* root);

short randoms[MAX_RAND];

MCTSnode* MCTS_test (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int maxprocs) {
    
	node 							outBoard = board;
	struct timeval					currentTime;
	MCTSnode 						*root, *selectedNode, *tempNode;
	char 							chars[30] = "abcdefghijklmnopqrstuwvxyz";
	tasks							tempChildren;
	short							randTemp;
	short							simResult;
	short							timeLeft = 1;
	double							timeElapsed;
	int							i;
    
	root = (MCTSnode*) malloc (sizeof(MCTSnode));
	
	if (root == NULL) {
#ifndef CURSES
		printf("MCTS: Root malloc problem\n");
#endif
		exit(0);
	}
	
	initMCTSnode(&root, root, masterPlayer, board, NULL, 0);
    
	root->active = 1;
	
	selectedNode = root;
	
	
	while (timeLeft) {
        
        
		//////////////////////////////////
		//expand children
		//////////////////* helpful macro for catching CUDA errors */
#define CUDA_SAFE_CALL(call) do{\
cudaError_t err = call ; \
if (cudaSuccess != err){\
fprintf(stderr, "cuda error at %s:%d, %s\n",\
__FILE__, __LINE__, cudaGetErrorString(err));\
}\
}while(0)
        /////////////////
		if (selectedNode->childrenCreated == 0) {
			
			tempChildren = getChildren (selectedNode->who, selectedNode->board);
			selectedNode->childrenNo = tempChildren.howMany;
			selectedNode->childrenCreated = 1;
			
			
			if (selectedNode->childrenNo > 0) {
                
				selectedNode->children = (MCTSnode*)calloc(selectedNode->childrenNo, sizeof (MCTSnode));
                
				for (i = 0; i < tempChildren.howMany; i++) {
                    
                    tempNode = ((MCTSnode*)(selectedNode->children+i));
                    
                    initMCTSnode(&tempNode, selectedNode, masterPlayer, tempChildren.taskList[i], chars, i);
                    
                    
				}
			}
            
		}
		
		//pick first random from selected
		if (selectedNode->childrenNo > 0) {
            
			randTemp = rand() % selectedNode->childrenNo;
			getChild(selectedNode, randTemp)->active = 1;
            
			//simulate
			if (id < maxprocs) {
				simResult = performRandomSimulation (getChild(selectedNode, randTemp)->board, getChild(selectedNode, randTemp)->who, depth, masterPlayer);
				
				
				getChild(selectedNode, randTemp)->simulations++;
				getChild(selectedNode, randTemp)->success += simResult;
				
				backPropagate(getChild(selectedNode, randTemp), 1, simResult);
			}
			
		}
		
		//checkTime
		gettimeofday(&currentTime, 0);
		timeElapsed = getTimeDiff(&startTime, &currentTime);
        
        
		//printf("id = %d, %f\n", id, timeElapsed);
		if (timeElapsed >= maxTime)
			timeLeft = 0;
		
		
		//selectNode for next iter
		
		selectedNode = selectNode(root, C, NULL);
		
		//time loop end
		
	}
	
	
	return root;
    
}

void setDevice(short device) {
    
    cudaSetDevice(device);
    
}

MCTSnode* MCTS_GPU (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int maxprocs, short b, short t, short device) {
    
	node 							outBoard = board;
	struct timeval						currentTime;
	MCTSnode 						*root, *selectedNode, *tempNode;
	char 							chars[30] = "abcdefghijklmnopqrstuwvxyz";
	tasks							tempChildren;
	short							randTemp;
	short							simResult;
	short							timeLeft = 1;
	double							timeElapsed;
	short							gpuid = 0;
	short							blocks = b;
	short							thread_no = t;
	short							no_elements = blocks * thread_no;
	short							GPU_THREADS = no_elements;
	void							*hostIn, *hostRand, *hostOut, *iMem, *rMem, *oMem;
	short							i;
	int							totalSim = 0;
    
	
    //if (id == 0) { //dla pierwszego gpu
	if (blocks < 0) blocks = 1;
	if (thread_no < 0 || thread_no > 1024) thread_no = 128;
	root = (MCTSnode*) malloc (sizeof(MCTSnode));
	
	printf("dev = %d GPU B: %d, T: %d\n", device, blocks, thread_no);
	if (root == NULL) {
#ifndef CURSES
		printf("MCTS: Root malloc problem\n");
#endif
		exit(0);
	}
	
	initMCTSnode(&root, root, masterPlayer, board, NULL, 0);
    
	root->active = 1;
	
	selectedNode = root;
	
	//cudaSetDevice (gpuid);
    
    CUDA_SAFE_CALL(cudaMallocHost( (void**) &(hostIn), no_elements * sizeof(node)));
    CUDA_SAFE_CALL(cudaMallocHost( (void**) &(hostRand), (no_elements+256) * sizeof(short)));
    CUDA_SAFE_CALL(cudaMallocHost( (void**) &(hostOut),  no_elements * sizeof(int)));
    CUDA_SAFE_CALL(cudaMalloc( (void**) &(iMem),   no_elements  * sizeof(node)));
    CUDA_SAFE_CALL(cudaMalloc( (void**) &(rMem),  (no_elements+256)  * sizeof(short)));
    CUDA_SAFE_CALL(cudaMalloc( (void**) &(oMem), no_elements * sizeof(int)));
    
	dim3 threads(thread_no, 1, 1);
	dim3 grid(blocks,1,1);
    
	//for (int i = 0 ; i <  MAX_RAND; i++)
	//randoms[i][g] =
    
	for (int i = 0 ; i <  no_elements; i++)
		((short*)(hostRand))[i] = rand() % 32;//randoms[i];
    
	int step = 0;
	while (timeLeft)	 {
        
        //	printf("step: %d\n", ++step);
        
        //////////////////////////////////
		//expand children
		//////////////////////////////////
		if (selectedNode->childrenCreated == 0) {
			
			tempChildren = getChildren (selectedNode->who, selectedNode->board);
			selectedNode->childrenNo = tempChildren.howMany;
			selectedNode->childrenCreated = 1;
			
			
			if (selectedNode->childrenNo > 0) {
                
				selectedNode->children = (MCTSnode*)calloc(selectedNode->childrenNo, sizeof (MCTSnode));
                
				for (i = 0; i < tempChildren.howMany; i++) {
                    
                    tempNode = ((MCTSnode*)(selectedNode->children+i));
                    
                    initMCTSnode(&tempNode, selectedNode, masterPlayer, tempChildren.taskList[i], chars, i);
                    
                    
				}
			}
            
		}
		
		//pick first random from selected
		if (selectedNode->childrenNo > 0) {
            
			randTemp = rand() % selectedNode->childrenNo;
			getChild(selectedNode, randTemp)->active = 1;
            
			//simulate
			if (id < maxprocs) {
                
                if (step > 100)
                    simResult = performRandomSimulation (getChild(selectedNode, randTemp)->board, getChild(selectedNode, randTemp)->who, depth, masterPlayer); else {
                        
                        for (int b = 0; b < blocks; b++)
                            ((node*)(hostIn))[b*thread_no] = getChild(selectedNode, randTemp)->board;
                        ((short*)(hostRand))[no_elements + 3] = getChild(selectedNode, randTemp)->who;
                        ((short*)(hostRand))[no_elements + 1] =  depth;
                        ((short*)(hostRand))[no_elements + 2] = masterPlayer;
                        CUDA_SAFE_CALL( cudaMemcpy( iMem , hostIn, sizeof(node)* no_elements, cudaMemcpyHostToDevice));
                        
                        CUDA_SAFE_CALL( cudaMemcpy( rMem , hostRand, sizeof(short) * (no_elements+256), cudaMemcpyHostToDevice));
                        
                        
                        
                        //printf("kernel\n");
                        MCTS <<< grid, threads >>> ( ((node*)iMem),  (short*) (oMem) , (short)who, (short*)rMem);
                        checkCUDAError("kernel execution");
                        
                        CUDA_SAFE_CALL( cudaMemcpy( hostOut, oMem, sizeof(short) * no_elements, cudaMemcpyDeviceToHost));
                    }
                int res = 0;
                
                for (int i = 0; i < no_elements; i++) {
                    //result[tempIndex]  += ((int*)t_in->hostOut)[i];
                    res += ((short*)hostOut)[i];
                    
                }
                totalSim += no_elements;
                //		printf("%d\n", totalSim);
                //printf("%d - %d, %d\n", res, no_elements, totalSim);
				getChild(selectedNode, randTemp)->simulations+= no_elements;
				getChild(selectedNode, randTemp)->success += res;//simResult;
				
				backPropagate(getChild(selectedNode, randTemp), no_elements, res);
			}
			
		}
		
		//checkTime
		gettimeofday(&currentTime, 0);
		timeElapsed = getTimeDiff(&startTime, &currentTime);
        
        
		//printf("id = %d, %f %ld, %ld\n", id, timeElapsed, ++iter, nodeCount(root));
		if (timeElapsed >= maxTime)
			timeLeft = 0;
		
		
		//selectNode for next iter
		
		selectedNode = selectNode(root, C, NULL);
		
		//time loop end
		
	}
	//printf("total %d\n", totalSim);
	CUDA_SAFE_CALL(cudaFree(iMem));
	CUDA_SAFE_CALL(cudaFree(oMem));
	CUDA_SAFE_CALL(cudaFree(rMem));
	CUDA_SAFE_CALL(cudaFreeHost(hostIn));
	CUDA_SAFE_CALL(cudaFreeHost(hostOut));
	CUDA_SAFE_CALL(cudaFreeHost(hostRand));
    //}
	return root;
    
}
#define MAX_BLOCKS 512
MCTSnode* MCTS_GPU_root (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int maxprocs, short b, short t, short device) {
    
    
	node 							outBoard = board;
	struct timeval						currentTime;
	MCTSnode 						*root[MAX_BLOCKS], *selectedNode[MAX_BLOCKS], *tempNode[MAX_BLOCKS];
	char 							chars[30] = "abcdefghijklmnopqrstuwvxyz";
	tasks							tempChildren[MAX_BLOCKS];
	short							randTemp[MAX_BLOCKS];
	short							simResult[MAX_BLOCKS];
	short							timeLeft = 1;
	double							timeElapsed;
	short							gpuid = 0;
	short							blocks = b;
	short							thread_no = t;
	short							no_elements = blocks * thread_no;
	short							GPU_THREADS = no_elements;
	void							*hostIn, *hostRand, *hostOut, *iMem, *rMem, *oMem;
	short							i;
	int							totalSim = 0;
    
	//cudaSetDevice(device);
    //if (id == 0) { //dla pierwszego gpu
	if (blocks < 0) blocks = 1;
	if (thread_no < 0 || thread_no > 1024) thread_no = 128;
	
	printf("[R]GPU B: %d, T: %d\n", blocks, thread_no);
	
	for (int b = 0; b < blocks; b++) {
        root[b] = (MCTSnode*) malloc (sizeof(MCTSnode));
        
        
        if (root[b] == NULL) {
#ifndef CURSES
            printf("MCTS: Root malloc problem\n");
#endif
            exit(0);
        }
        
        initMCTSnode(&(root[b]), root[b], masterPlayer, board, NULL, 0);
        
        root[b]->active = 1;
        
        
        selectedNode[b] = root[b];
	}
    //	printf("a\n");
	//cudaSetDevice (gpuid);
    
    CUDA_SAFE_CALL(cudaMallocHost( (void**) &(hostIn), no_elements * sizeof(node)));
    CUDA_SAFE_CALL(cudaMallocHost( (void**) &(hostRand), (no_elements+256) * sizeof(short)));
    CUDA_SAFE_CALL(cudaMallocHost( (void**) &(hostOut),  no_elements * sizeof(int)));
    CUDA_SAFE_CALL(cudaMalloc( (void**) &(iMem),   no_elements  * sizeof(node)));
    CUDA_SAFE_CALL(cudaMalloc( (void**) &(rMem),  (no_elements+256)  * sizeof(short)));
    CUDA_SAFE_CALL(cudaMalloc( (void**) &(oMem), no_elements * sizeof(int)));
    
	dim3 threads(thread_no, 1, 1);
	dim3 grid(blocks,1,1);
    
	//for (int i = 0 ; i <  MAX_RAND; i++)
	//randoms[i][g] =
    
	for (int i = 0 ; i <  no_elements; i++)
		((short*)(hostRand))[i] = rand() % 32;//randoms[i];
    
	int step = 0;
	//printf("b\n");
	while (timeLeft)	 {
        
        //	printf("step: %d\n", ++step);
        
        //////////////////////////////////
		//expand children
		//////////////////////////////////
		for (int b = 0; b < blocks; b++)
            if (selectedNode[b]->childrenCreated == 0) {
                
                tempChildren[b] = getChildren (selectedNode[b]->who, selectedNode[b]->board);
                selectedNode[b]->childrenNo = tempChildren[b].howMany;
                selectedNode[b]->childrenCreated = 1;
                
                
                if (selectedNode[b]->childrenNo > 0) {
                    
                    selectedNode[b]->children = (MCTSnode*)calloc(selectedNode[b]->childrenNo, sizeof (MCTSnode));
                    
                    for (i = 0; i < tempChildren[b].howMany; i++) {
                        
                        tempNode[b] = ((MCTSnode*)(selectedNode[b]->children+i));
                        
                        initMCTSnode(&tempNode[b], selectedNode[b], masterPlayer, tempChildren[b].taskList[i], chars, i);
                        
                        
                    }
                }
                
            }
		
		//pick first random from selected
		
		if (selectedNode[0]->childrenNo)
		{
            
            for (int b = 0; b < blocks; b++) {
                if (selectedNode[b]->childrenNo) {
                    randTemp[b] = rand() % selectedNode[b]->childrenNo;
                    getChild(selectedNode[b], randTemp[b])->active = 1;
                }
            }
            //simulate
            if (id < maxprocs) {
                
                if (step > 100)
                    for (int b = 0; b < blocks; b++) {
                        if (selectedNode[b]->childrenNo)
                            simResult[b] = performRandomSimulation (getChild(selectedNode[b], randTemp[b])->board, getChild(selectedNode[b], randTemp[b])->who, depth, masterPlayer);
                    }
                
                else {
					
                    for (int b = 0; b < blocks; b++)
                        if (selectedNode[b]->childrenNo)
                            ((node*)(hostIn))[b*thread_no] = getChild(selectedNode[b], randTemp[b])->board;
                        else
                            ((node*)(hostIn))[b*thread_no] = getChild(selectedNode[0], randTemp[0])->board;
                    ((short*)(hostRand))[no_elements + 3] = getChild(selectedNode[0], randTemp[0])->who;
                    ((short*)(hostRand))[no_elements + 1] =  depth;
                    ((short*)(hostRand))[no_elements + 2] = masterPlayer;
                    CUDA_SAFE_CALL( cudaMemcpy( iMem , hostIn, sizeof(node)* no_elements, cudaMemcpyHostToDevice));
                    
                    CUDA_SAFE_CALL( cudaMemcpy( rMem , hostRand, sizeof(short) * (no_elements+256), cudaMemcpyHostToDevice));
                    
                    
                    
                    //printf("kernel\n");
                    MCTS <<< grid, threads >>> ( ((node*)iMem),  (short*) (oMem) , (short)who, (short*)rMem);
                    checkCUDAError("kernel execution");
                    
                    CUDA_SAFE_CALL( cudaMemcpy( hostOut, oMem, sizeof(short) * no_elements, cudaMemcpyDeviceToHost));
                }
                int res[MAX_BLOCKS];
                for (int b = 0; b < blocks; b++) {
                    res[b] = 0;
                    for (int i = 0; i < thread_no; i++) {
                        //result[tempIndex]  += ((int*)t_in->hostOut)[i];
                        res[b] += ((short*)hostOut)[b*thread_no+i];
                        
                    }
                }
                totalSim += no_elements;
                //		printf("%d\n", totalSim);
                //printf("%d - %d, %d\n", res, no_elements, totalSim);
                for (int b = 0; b < blocks; b++) {
                    
                    if (selectedNode[b]->childrenNo)  {
                        getChild(selectedNode[b], randTemp[b])->simulations+= thread_no;
                        getChild(selectedNode[b], randTemp[b])->success += res[b];//simResult[0];
                        backPropagate(getChild(selectedNode[b], randTemp[b]), thread_no, res[b]);
                        
                    }
                }
            }
            
		}
		
		//checkTime
		gettimeofday(&currentTime, 0);
		timeElapsed = getTimeDiff(&startTime, &currentTime);
        
        
		//printf("id = %d, %f %ld, %ld\n", id, timeElapsed, ++iter, nodeCount(root[0]));
		if (timeElapsed >= maxTime)
			timeLeft = 0;
		
		
		//selectNode for next iter
		for (int b = 0; b < blocks; b++)
			selectedNode[b] = selectNode(root[b], C, NULL);
		
		//time loop end
		
	}
	//printf("total %d\n", totalSim);
	CUDA_SAFE_CALL(cudaFree(iMem));
	CUDA_SAFE_CALL(cudaFree(oMem));
	CUDA_SAFE_CALL(cudaFree(rMem));
	CUDA_SAFE_CALL(cudaFreeHost(hostIn));
	CUDA_SAFE_CALL(cudaFreeHost(hostOut));
	CUDA_SAFE_CALL(cudaFreeHost(hostRand));
    
	for (int i = 1 ; i <  blocks; i++) {
		
		for (int j = 0; j < root[0]->childrenNo; j++) {
            //printf("%d %d %d %d\n", i, j, (getChild(root[i],j)->success), (getChild(root[i],j)->simulations));
            (getChild(root[0],j)->success) += (double)(getChild(root[i],j)->success);
            (getChild(root[0],j)->simulations) += (double)(getChild(root[i],j)->simulations);
        }
        root[0]->simulations += root[i]->simulations;
        root[0]->success += root[i]->success;
        
		deleteMCTS(root[i]);
		delete(root[i]);
	}
	
	return root[0];
    
}

MCTSnode* MCTS (node board, short who, short masterPlayer, short depth, float maxTime, float C, struct timeval startTime, int id, int maxprocs) {
    
	node 							outBoard = board;
	struct timeval					currentTime;
	MCTSnode 						*root, *selectedNode, *tempNode;
	char 							chars[30] = "abcdefghijklmnopqrstuwvxyz";
	tasks							tempChildren;
	short							randTemp;
	short							simResult;
	short							timeLeft = 1;
	double							timeElapsed;
	short							i;
    
	root = (MCTSnode*) malloc (sizeof(MCTSnode));
	
	if (root == NULL) {
#ifndef CURSES
		printf("MCTS: Root malloc problem\n");
#endif
		exit(0);
	}
	
	initMCTSnode(&root, root, masterPlayer, board, NULL, 0);
    
	root->active = 1;
	
	selectedNode = root;
	
	while (timeLeft) {
        
        
		//////////////////////////////////
		//expand children
		//////////////////////////////////
		if (selectedNode->childrenCreated == 0) {
			
			tempChildren = getChildren (selectedNode->who, selectedNode->board);
			selectedNode->childrenNo = tempChildren.howMany;
			selectedNode->childrenCreated = 1;
			
			
			if (selectedNode->childrenNo > 0) {
                
				selectedNode->children = (MCTSnode*)calloc(selectedNode->childrenNo, sizeof (MCTSnode));
                
				for (i = 0; i < tempChildren.howMany; i++) {
                    
                    tempNode = ((MCTSnode*)(selectedNode->children+i));
                    
                    initMCTSnode(&tempNode, selectedNode, masterPlayer, tempChildren.taskList[i], chars, i);
                    
                    
				}
			}
            
		}
		
		//pick first random from selected
		if (selectedNode->childrenNo > 0) {
            
			randTemp = rand() % selectedNode->childrenNo;
			getChild(selectedNode, randTemp)->active = 1;
            
			//simulate
			if (id < maxprocs) {
				simResult = performRandomSimulation (getChild(selectedNode, randTemp)->board, getChild(selectedNode, randTemp)->who, depth, masterPlayer);
				
				
				getChild(selectedNode, randTemp)->simulations++;
				getChild(selectedNode, randTemp)->success += simResult;
				
				backPropagate(getChild(selectedNode, randTemp), 1, simResult);
			}
			
		}
		
		//checkTime
		gettimeofday(&currentTime, 0);
		timeElapsed = getTimeDiff(&startTime, &currentTime);
        
        
		//printf("id = %d, %f %ld, %ld\n", id, timeElapsed, ++iter, nodeCount(root));
		if (timeElapsed >= maxTime)
			timeLeft = 0;
		
		
		//selectNode for next iter
		
		selectedNode = selectNode(root, C, NULL);
		
		//time loop end
		
	}
	
	
	return root;
    
}
MCTSnode* selectNode(MCTSnode* root, float C, MCTSnode* best) {
    
	MCTSnode* tempNode = NULL;
	short i;
    
	if (best == NULL) best = root;
	
	if (root->active)
		for (i = 0; i < root->childrenNo; i++) {
            
			if (calculateMCTSVal(getChild(root,i),C) > calculateMCTSVal(best,C))
				best = getChild(root,i);
			
			tempNode = selectNode(getChild(root,i), C, best);
			
			if (tempNode != NULL) {
                
				if (calculateMCTSVal(tempNode,C) > calculateMCTSVal(best,C))
                    best = tempNode;
			}
            
		}
	
	return best;
    
}

long nodeCount(MCTSnode* root) {
    
	long no = 1;
	short i;
    
	if (root->active)
		for (i = 0; i < root->childrenNo; i++) {
            
			no += nodeCount(getChild(root,i));
		}
	
	return no;
    
}


void backPropagate(MCTSnode* leaf, long sim, long suc) {
    
    
	if (leaf->parent != leaf) {
		leaf->parent->simulations += sim;
		leaf->parent->success += suc;
		backPropagate(leaf->parent, sim, suc);
	}
    
}

void deleteMCTS (MCTSnode* root) {
    
	short i;
	if (root)
        if (root->children != NULL) {
            
            for (i = 0; i < root->childrenNo; i++)
                deleteMCTS (getChild(root, i));
            
            free(root->children);
        }
    
}

void initMCTSnode(MCTSnode **node, MCTSnode * parent, short masterPlayer, struct node_struct board, char* chars, short childNum) {
    
    
	((MCTSnode*)*node)->simulations = 2;
	((MCTSnode*)*node)->success = 1;
	((MCTSnode*)*node)->children = NULL;
	((MCTSnode*)*node)->childrenCreated = 0;
	((MCTSnode*)*node)->childrenNo = 0;
	((MCTSnode*)*node)->parent = parent;
	((MCTSnode*)*node)->active = 0;
	((MCTSnode*)*node)->board = board;
	
	if (((MCTSnode*)*node) != parent) {
        
		((MCTSnode*)*node)->who = !(parent->who);
		((MCTSnode*)*node)->depth = parent->depth + 1;
		strncpy(((MCTSnode*)*node)->id, parent->id, ((MCTSnode*)*node)->depth);
		((MCTSnode*)*node)->id[((MCTSnode*)*node)->depth] = chars[childNum];
        
	}
	else {
        
		((MCTSnode*)*node)->who = masterPlayer;
		((MCTSnode*)*node)->depth = 0;
		((MCTSnode*)*node)->id[0] = 'R';
		((MCTSnode*)*node)->id[1] = '\0';	
        
	}
    
	
}

void printMCTSnode (MCTSnode* node, float C, short includeChildren) {
    
    short i;
#ifndef CURSES
	//printf("ID: %s, Who: %d, Depth: %d, Active: %d, %ld/%ld (%.2f), ChildrenNo: %d, Avg: %.2f, Val: %.2f\n", node->id, node->who, node->depth, node->active, node->success, node->simulations, (float)node->success/(float)node->simulations, node->childrenNo, getAverage(node), calculateMCTSVal(node, C));
	if (includeChildren) {
        
        //	printf("Children of %s:\n", node->id);
		
		for (i = 0; i < node->childrenNo; i++)
			printMCTSnode(getChild(node,i), C, 0);
        
	}
#endif
}

void MCTSTest(cmdArguments args, int argc, const char **argv) {
    
	struct timeval 		startTime, currentTime;
    node				board;
    int 				player, masterPlayer;
    
	gettimeofday(&startTime, 0);
    
#ifndef CURSES	
	printf("[MCTS Test Start] Time: %s\n", getCurrentLocalTime(&startTime));
#endif
	board = initBoard();
	player = START_PLAYER;
	masterPlayer = START_PLAYER;
	
	initRand ();
	MCTSnode* root = MCTS (board, player, masterPlayer, args.depth, args.maxTime, args.C, startTime, 0, 1); 
	
	printMCTSnode(root, args.C, 1);
	deleteMCTS(root);
	delete(root);
	gettimeofday(&currentTime, 0);
    
#ifndef CURSES
	printf("[MCTS Test End] Time: %s\n", getCurrentLocalTime(&currentTime));
#endif
}
