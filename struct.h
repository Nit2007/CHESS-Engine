#pragma once
#include "defs.h"

struct s_undo
{
    int move;
    int castleperm;
    int enpas;
    int fifty;
    uint64_t poskey;
};

struct s_board
{
    int pieces[120];//BOARD_SQ_NUM
    
    uint64_t pawns[3];
    int king[3];
    int bigpce[3];
    int majpce[3];
    int minpce[3];
    int material[3];

    int side;
    int enpas;
    int fifty;
    
    int ply;          //NUMBER OF HALF MOVES PLAYED IN THE CURRENT SEARCH ( resets to 0 in a new search )
    int hisply;       // total NUMBER OF half HISTORY MOVES PLAYED IN THE WHOLE GAME
    int castleperm;
    
    uint64_t poskey;

    int piecelist[13][10];
    int piecenum[13];
    s_undo history[2048];//MAXGAMEMOVES//vector<s_undo>history(MAXGAMEMOVES);
};
