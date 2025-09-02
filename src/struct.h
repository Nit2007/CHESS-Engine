#pragma once
#include "defs.h"

struct s_move
{
    int move;
    int score;
};

struct s_movelist
{
    s_move moves[256];
    int count;
};

struct s_undo
{
    int move;
    int castleperm;
    int enpas;
    int fifty;
    uint64_t poskey;
};

struct s_hashentry{
	U64 posKey;
	int move;
	/*int score;
	int depth;
	int flags;*/
};
 struct s_hashtable{
	s_hashentry *pTable;
	int numEntries;
	/*int newWrite;
	int overWrite;
	int hit;
	int cut;*/
};

struct s_board
{// Board representation using 120-square "mailbox" format (10x12 board)
    int pieces[120];//BOARD_SQ_NUM   // Stores the piece type on each square. Indexed using 120-square layout.

    uint64_t pawns[3];//  Bitboards for pawns:[3] represents WHITE , BLACK and BOTH, Used for fast pawn-specific operations.
    int king[3];
    int bigpce[3];//excluding pawns
    int majpce[3];// R , Q , K
    int minpce[3];// N , B
    int material[3]; // Material score in centipawns for each side

    int side;
    int enpas;
    int fifty;
    
    int ply;          //NUMBER OF HALF MOVES PLAYED IN THE CURRENT SEARCH ( resets to 0 in a new search )
    int hisply;       // total NUMBER OF half HISTORY MOVES PLAYED IN THE WHOLE GAME
    int castleperm;
    
    uint64_t poskey;

    int piecelist[13][10]; // [piece][count] => square. Example: piecelist[WN][0] = E4
    int piecenum[13]; // Total count of each piece type
    s_undo history[2048];//MAXGAMEMOVES//vector<s_undo>history(MAXGAMEMOVES);

    s_hashtable hashtable[1];
};

