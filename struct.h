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
    int pieces[BOARD_SQ_NUM];
    
    uint64_t pawns[3];
    int king[3];
    int bigpce[3];
    int majpce[3];
    int minpce[3];

    int side;
    int enpas;
    int fifty;
    
    int ply;
    int hisply;
    int castleperm;
    
    uint64_t poskey;

    int piecelist[13][10];
    s_undo history[MAXGAMEMOVES];//vector<s_undo>history(MAXGAMEMOVES);
};
