#include "defs.h"
#include "struct.h"

int square120[8][8];
//FOR CONVERTING CHESS SQUARE NAME TO (FILE,RANK)
void initsquare120(){
for(int rank=0;rank<8;rank++)
    {
        for(int file=0;file<8;file++)
            {
                square120[rank][file]=smalltobig(file,rank);
            }
    }
}
int fileCharToIndex(char c) {
    return tolower(c) - 'a'; // 'a' = 0
}

int rankCharToIndex(char c) {
    return c - '1'; // '1' = 0
}

int getSquareFromString(const string& sq) {
    if (sq.length() != 2) return -1;
    int file = fileCharToIndex(sq[0]);
    int rank = rankCharToIndex(sq[1]);
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return square120[rank][file];
}

uint64_t PieceKeys[13][120];
uint64_t SideKey;
uint64_t CastleKeys[16];

void InitHashKeys() {
   for (int index = 0; index < 13; ++index) {
        for (int index2 = 0; index2 < 120; ++index2) {
            PieceKeys[index][index2] = RAND_64;
        }
    }
SideKey = RAND_64;
    for (int index = 0; index < 16; ++index) {
        CastleKeys[index] = RAND_64;
    }
}

uint64_t GeneratePosKey(const s_board* pos) {
    int sq = 0;
    int piece = EMPTY;
    uint64_t finalKey = 0;

    for (sq = 0; sq < BOARD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if (piece != NO_SQ && piece != EMPTY) {
            ASSERT(piece >= WP && piece <= BK);
            finalKey ^= PieceKeys[piece][sq];
        }
    }

    if (pos->side == WHITE) {
        finalKey ^= SideKey;
    }

    if (pos->enpas != NO_SQ) {
        ASSERT(pos->enpas >= 0 && pos->enpas < BOARD_SQ_NUM);
        finalKey ^= PieceKeys[EMPTY][pos->enpas];
    }

    ASSERT(pos->castleperm >= 0 && pos->castleperm <= 15);
    finalKey ^= CastleKeys[pos->castleperm];

    return finalKey;
}

void allinit()
{
    init120to64();
    initsquare120();
    InitHashKeys();
}
