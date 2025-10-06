#include "defs.h"

int pieceBig[13]={false,false,true,true,true,true,true,false,true,true,true,true,true};//excluding pawns
int pieceMaj[13]={false,false,false,false,true,true,true,false,false,false,true,true,true};// R , Q , K
int pieceMin[13]={false,false,true,true,false,false,false,false,true,true,false,false,false};// N , B
int pieceVal[13]={0,100,325,325,500,1000,50000,100,325,325,500,1000,50000};
int pieceCol[13]={BOTH,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};

int PiecePawn[13]        = { FALSE, TRUE , FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , FALSE, FALSE, FALSE, FALSE, FALSE };	
int PieceKnight[13]      = { FALSE, FALSE, TRUE , FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , FALSE, FALSE, FALSE, FALSE };
int PieceKing[13]        = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , FALSE, FALSE, FALSE, FALSE, FALSE, TRUE  };
int PieceRookQueen[13]   = { FALSE, FALSE, FALSE, FALSE, TRUE , TRUE , FALSE, FALSE, FALSE, FALSE, TRUE , TRUE , FALSE };
int PieceBishopQueen[13] = { FALSE, FALSE, FALSE, TRUE , FALSE, TRUE , FALSE, FALSE, FALSE, TRUE , FALSE, TRUE , FALSE };
int PieceSlides[13]      = { FALSE, FALSE, FALSE, TRUE , TRUE, TRUE , FALSE, FALSE, FALSE, TRUE , TRUE , TRUE , FALSE };


int MvvLvaScores[13][13];
// Piece values for MVV-LVA (using  existing piece values but reorganized)
int MvvLvaValues[13] = {0, 100, 325, 325, 500, 1000, 50000, 100, 325, 325, 500, 1000, 50000};

uint64_t IsolatedMask[64];

void InitIsolatedMask() {
    for (int sq = 0; sq < 64; sq++) {
        int file = sq % 8;  // 0=a, 7=h
        uint64_t mask = 0;
        if (file > 0) mask |= (1ULL << (file - 1));  // Left file
        if (file < 7) mask |= (1ULL << (file + 1));  // Right file
        IsolatedMask[sq] = mask;
    }
}