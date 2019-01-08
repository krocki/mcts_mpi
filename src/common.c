#include "headers.h"

node makeRandomMove (node board, short who) {
    
    
	tasks t = getChildren (who, board);
    
	if (t.howMany > 0)
        return t.taskList[rand() % t.howMany];
    
	else return board;
    
}

int performRandomSimulation (node startBoard, short who, short maxDepth, int master_player) {
    
	node board = startBoard;
	int player = who;
    
	int iter = 0;
	
	while (1) {
        
		iter++;
        
		
        
		if (( getChildren (player,board).howMany) > 0)
            board = makeRandomMove (board, player);
		player = !player;
		
		if ( ((( getChildren (player,board).howMany) < 1) && (( getChildren (!player,board).howMany) < 1)) || iter > maxDepth)
            
            break;
        
	}
    
	//if (scoreCPU (board, master_player) > scoreCPU (board, !master_player)) return 1;
    
	//else
	return 64 - scoreCPU (board, !master_player);//return 0;
    
}

float getTimeDiff(struct timeval *s, struct timeval *e) {
    
	struct timeval	diff_tv;
    
	diff_tv.tv_usec = e->tv_usec - s->tv_usec;
	diff_tv.tv_sec = e->tv_sec - s->tv_sec;
    
	if (s->tv_usec > e->tv_usec)
	{
		diff_tv.tv_usec += 1000000;
		diff_tv.tv_sec--;
	}
    
	return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
}

void printTypeData(void) {
    
	printf("Size of node = %ld Byte(s)\n", sizeof(node));
	printf("Size of short = %ld Byte(s)\n", sizeof(short));
	printf("Size of int = %ld Byte(s)\n", sizeof(short));
	printf("Size of char = %ld Byte(s)\n", sizeof(char));
    
}

void printBits(char byte, char size) {
    
	short i;
	
	for (i = 0; i < size; i++) {
        
		printf("%d", (byte & 1));
		byte >>= 1;
	}
}

void initRand (void) {
    
	struct timeval us;
	gettimeofday (&us, NULL);
	srand ((unsigned int) us.tv_usec);
    
}

int checkEnd(node board) {
    
	if ((( getChildren (START_PLAYER,board).howMany) < 1) && (( getChildren (!START_PLAYER, board).howMany) < 1))
        
        return 1;
	else return 0;
    
}

char* getCurrentLocalTime(struct timeval *s) {
    
	//char* temp = NULL;
	
	/*temp = (char*)malloc(sizeof(char) * 128);
     
     time_t tt = (time_t)s->tv_sec;
     
     struct tm *sec;
     sec = localtime(&tt);
     
     sprintf(temp, "%d.%d %d:%d:%d.%06ld", sec->tm_mon, sec->tm_mday, sec->tm_hour, sec->tm_min, sec->tm_sec, s->tv_usec);
     
     */
	return "NOTIME";//temp;
	
}

void printCurrentLocalTime(struct timeval *s) {
    
	
	time_t tt = (time_t)s->tv_sec;
   	
   	struct tm *sec;
    sec = localtime(&tt);
    
	printf("[%d.%d %d:%d:%d.%04ld] ", sec->tm_mon, sec->tm_mday, sec->tm_hour, sec->tm_min, sec->tm_sec, s->tv_usec);
    
}


char* printMCTSResult(MPI_MCTS_Result* res) {
    
	char* out = (char*)malloc(sizeof(char) * 1024);
	char temp[128] = "";
	int i;
    
	if (res != NULL) {
		//clear
		strcpy(out,temp);
        
		sprintf(temp,"[Root] %ld/%ld = %.2f ", res->rSuccess, res->rTotal, res->rAverage);
		
		strcat(out, temp);
        
		for (i = 0; i < res->no; i++) {
			sprintf(temp,"[Child %d] %ld/%ld = %.2f ", i, res->success[i], res->simulations[i], res->average[i]);
			strcat(out, temp);
            
		}
	} else sprintf(out, "NULL");
    
	return out;
}

