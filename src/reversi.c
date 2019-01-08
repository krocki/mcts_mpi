#include "headers.h"

short scoreCPU (node board, int player) {
	
short score = 0;
int x,y;

	for (x = 0; x < 8; x++) 
	for (y = 0; y < 8; y++) 
	score += BOARD_GET_SCORE(board, x, y, player) -  BOARD_GET_SCORE(board, x, y, !player);

	return score;
	
}

#define CHECK_DIR(x,y,who) \
	if (!IS_OCCUPIED(board, x, y))\
	{\
		dirs = move(board, x, y, who);\
		if (IS_SET_DIR_ANY(dirs) && MAX_TASKS_STATIC > t.howMany)\
		t.taskList[(short)(t.howMany++)] = flip(board, dirs, x, y, who);\
	} 
	
tasks getChildren (int who, node board) {
	
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

node flip (node board, int dirs, int x, int y, int player) {
	
	int i;
	
	BOARD_SET_PLAYER(board, x, y, player);
	
	if ( IS_SET_DIR_DOWN(dirs) ) 
	for (i = x; !BOARD_IS_SET(board, x + 1, y, player); i++)
	BOARD_FLIP(board, i + 1, y);
	
	if ( IS_SET_DIR_UP(dirs) )
	for (i = x; !BOARD_IS_SET(board, x - 1, y, player); i--)
	BOARD_FLIP(board, i - 1, y);
	
	if ( IS_SET_DIR_LEFT(dirs) )
	for (i = y; !BOARD_IS_SET(board, x, y - 1, player); i--)
	BOARD_FLIP(board, x, i - 1);
	
	if ( IS_SET_DIR_RIGHT(dirs) )
	for (i = y; !BOARD_IS_SET(board, x, y + 1, player); i++)
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

int move (node board, int x, int y, int player) {
	
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

node initBoard (void) {

	node board;
	int i,j;
	
	//clear
	for (j = 0; j < 8; j++)
	for (i = 0; i < 8; i++)
	BOARD_CLEAR(board, j, i);

	//black
	BOARD_SET_PLAYER(board, 4, 4, BLACK);
	BOARD_SET_PLAYER(board, 3, 3, BLACK);

    //white
	BOARD_SET_PLAYER(board, 3, 4, WHITE);
	BOARD_SET_PLAYER(board, 4, 3, WHITE);

	return board;
}

void printBoard (node board) {

	int k,l;
    
    printf (" +-A-B-C-D-E-F-G-H+\n");
    
	for (k = 0; k < 8; k++)
    {

		printf ("%d|", k);

		for (l = 0; l < 8; l++)
		if (BOARD_IS_SET(board, k, l, BLACK))
		printf("-1");
		else if (BOARD_IS_SET(board, k, l, WHITE))
		printf(" 1");
		else printf(" 0");
		printf ("|%d\n", k);

    }

    printf (" +-A-B-C-D-E-F-G-H+\n");

}
