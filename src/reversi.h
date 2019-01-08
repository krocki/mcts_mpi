#ifndef _NODETYPE
#define _NODETYPE
#define BIT_NODE


//bitwise operations macros
#define DIR_UP			BIT_0
#define DIR_UP_RIGHT	BIT_1
#define DIR_RIGHT		BIT_2
#define DIR_DOWN_RIGHT	BIT_3
#define DIR_DOWN		BIT_4
#define DIR_DOWN_LEFT	BIT_5
#define DIR_LEFT		BIT_6
#define DIR_UP_LEFT		BIT_7
#define DIR_ANY			ALL_1

#define IS_SET_DIR_UP(dir)				(dir & DIR_UP)
#define IS_SET_DIR_UP_RIGHT(dir)			(dir & DIR_UP_RIGHT)
#define IS_SET_DIR_RIGHT(dir)				(dir & DIR_RIGHT)
#define IS_SET_DIR_DOWN_RIGHT(dir)			(dir & DIR_DOWN_RIGHT)
#define IS_SET_DIR_DOWN(dir)				(dir & DIR_DOWN)
#define IS_SET_DIR_DOWN_LEFT(dir)			(dir & DIR_DOWN_LEFT)
#define IS_SET_DIR_LEFT(dir)				(dir & DIR_LEFT)
#define IS_SET_DIR_UP_LEFT(dir)				(dir & DIR_UP_LEFT)
#define IS_SET_DIR_ANY(dir)				(dir & DIR_ANY)
#define SET_DIR_UP(dir)					(dir |= DIR_UP)
#define SET_DIR_UP_RIGHT(dir)				(dir |= DIR_UP_RIGHT)
#define SET_DIR_RIGHT(dir)				(dir |= DIR_RIGHT)
#define SET_DIR_DOWN_RIGHT(dir)				(dir |= DIR_DOWN_RIGHT)
#define SET_DIR_DOWN(dir)				(dir |= DIR_DOWN)
#define SET_BIT_DIR_DOWN(dir, bit)			(dir |= DIR_DOWN & (bit & BIT_4))
#define SET_DIR_DOWN_LEFT(dir)				(dir |= DIR_DOWN_LEFT)
#define SET_DIR_LEFT(dir)				(dir |= DIR_LEFT)
#define SET_DIR_UP_LEFT(dir)				(dir |= DIR_UP_LEFT)
#define RESET_DIR(dir)					(dir = 0x00)
#define BIT_IS_SET(byte,x)				(byte & (1 << x))

#ifdef BIT_NODE

#define BOARD_IS_BLACK(board, x, y)				(IS_OCCUPIED(board, x, y) && ((board.blackOrWhite[x] & (1 << (y))) == 0))
#define BOARD_IS_WHITE(board, x, y)				(IS_OCCUPIED(board, x, y) && ((board.blackOrWhite[x] & (1 << (y))) == (1 << (y))))
#define IS_OCCUPIED(board, x, y)				((board.occupied[x] & (1 << (y))) == (1 << (y)))
#define IS_OCCUPIED_SIGN(board, x, y)			(IS_OCCUPIED(board, x, y))

#define BOARD_SET_PLAYER(board, x, y, player)	{\
(board.occupied[x] |= (1 << (y)));\
(board.blackOrWhite[x] &= ~(1 << (y)));\
(board.blackOrWhite[x] |= (player << (y)));}
#define BOARD_FLIP(board, x, y)				{ (board.blackOrWhite[x] ^= (1 << (y)));}
#define BOARD_CLEAR(board, x, y)			{ (board.blackOrWhite[x] &= ~(1 << (y))); (board.occupied[x] &= ~(1 << (y))); }
#define BOARD_SET_BLACK(board, x, y)			{BOARD_SET_PLAYER(board, x, y, BLACK);}
#define BOARD_SET_WHITE(board, x, y)			{BOARD_SET_PLAYER(board, x, y, WHITE);}
#define BOARD_GET_SCORE(board, x, y, player)   	(BOARD_IS_SET(board, x, y, player))

#define BOARD_IS_SET(board, x, y, player)		(IS_OCCUPIED(board, x, (y)) && (board.blackOrWhite[x] & (1 << (y))) == (player << (y)))


typedef struct node_struct {
    
    char blackOrWhite[8];
    char occupied[8];
    
} node;

#endif

#ifndef BIT_NODE

#define IS_OCCUPIED(board, x, y)				(abs(board.blackOrWhite[x][y]) != 0 )
#define IS_OCCUPIED_SIGN(board, x, y)			(abs(board.blackOrWhite[x][y]) != 0 )
#define BOARD_FLIP(board, x, y)					{board.blackOrWhite[x][y] = -board.blackOrWhite[x][y];}
#define BOARD_CLEAR(board, x, y)				{board.blackOrWhite[x][y] = 0;}
#define BOARD_SET_BLACK(board, x, y)			{board.blackOrWhite[x][y] = -1;}
#define BOARD_SET_WHITE(board, x, y)			{board.blackOrWhite[x][y] = 1;}
#define BOARD_IS_BLACK(board, x, y)				((board.blackOrWhite[x][y] == -1))
#define BOARD_IS_WHITE(board, x, y)				((board.blackOrWhite[x][y] == 1))
#define BOARD_IS_SET(board, x, y, player)   	(board.blackOrWhite[x][y] == (2*player-1) )
#define BOARD_GET_SCORE(board, x, y, player)   	(board.blackOrWhite[x][y]* (2*player-1))
#define BOARD_SET_PLAYER(board, x, y, player)	(board.blackOrWhite[x][y] = (2*player-1))

typedef struct node_normal {
    
    char blackOrWhite[8][8];
    
} node;

#endif
#endif
