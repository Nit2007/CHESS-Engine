#include "defs.h"
#include "struct.h"

// Evaluation constants
const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;
const int CastleBonus = 50;
const int UncastledPenalty = 30;     // penalty for staying uncastled in opening (stronger)
const int EarlyQueenPenalty = 15;     // penalty for early queen development (stronger)
const int HangingBasePenalty = 20;    // base penalty added to a fraction of piece value when hanging (stronger)   // base penalty added to a fraction of piece value when hanging (stronger)
const int DevMinorBonus = 12;         // bonus for minor pieces developed in opening
const int KnightRimPenalty = 15;      // penalty for knights on rim in opening

// Piece-square tables (from white's perspective)
const int PawnTable[64] = {
    0,   0,   0,   0,   0,   0,   0,   0,  // Rank 8
  200, 200, 200, 200, 200, 200, 200, 200,  // Rank 7 (passed pawn push is very strong!)
   90, 100, 100, 120, 120, 100, 100,  90,  // Rank 6
   40,  50,  50,  80,  80,  50,  50,  40,  // Rank 5
   10,  20,  20,  50,  50,  20,  20,  10,  // Rank 4
    5,  10,  10,  20,  20,  10,  10,   5,  // Rank 3
    0,   0,   0, -10, -10,   0,   0,   0,  // Rank 2 (slight penalty, undeveloped)
    0,   0,   0,   0,   0,   0,   0,   0   // Rank 1 (start)
};


const int KnightTable[64] = {
     0,  -10,    0,    0,    0,    0,  -10,    0,
     0,    0,    0,    5,    5,    0,    0,    0,
     0,    0,   10,   10,   10,   10,    0,    0,
     0,    0,   10,   20,   20,   10,    5,    0,
     5,   10,   15,   20,   20,   15,   10,    5,
     5,   10,   10,   20,   20,   10,   10,    5,
     0,    0,    5,   10,   10,    5,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0
};

const int BishopTable[64] = {
     0,    0,  -10,    0,    0,  -10,    0,    0,
     0,    0,    0,   10,   10,    0,    0,    0,
     0,    0,   10,   15,   15,   10,    0,    0,
     0,   10,   15,   20,   20,   15,   10,    0,
     0,   10,   15,   20,   20,   15,   10,    0,
     0,    0,   10,   15,   15,   10,    0,    0,
     0,    0,    0,   10,   10,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0
};

const int RookTable[64] = {
     0,    0,    5,   10,   10,    5,    0,    0,
     0,    0,    5,   10,   10,    5,    0,    0,
     0,    0,    5,   10,   10,    5,    0,    0,
     0,    0,    5,   10,   10,    5,    0,    0,
     0,    0,    5,   10,   10,    5,    0,    0,
     0,    0,    5,   10,   10,    5,    0,    0,
    25,   25,   25,   25,   25,   25,   25,   25,
     0,    0,    5,   10,   10,    5,    0,    0
};

const int QueenTable[64] = {
   -20,  -10,  -10,   -5,   -5,  -10,  -10,  -20,
   -10,    0,    0,    0,    0,    0,    0,  -10,
   -10,    0,    5,    5,    5,    5,    0,  -10,
    -5,    0,    5,    5,    5,    5,    0,   -5,
     0,    0,    5,    5,    5,    5,    0,   -5,
   -10,    5,    5,    5,    5,    5,    0,  -10,
   -10,    0,    5,    0,    0,    0,    0,  -10,
   -20,  -10,  -10,   -5,   -5,  -10,  -10,  -20
};

// King tables for endgame and opening/middlegame
const int KingEndgame[64] = {
   -50,  -10,    0,    0,    0,    0,  -10,  -50,
   -10,    0,   10,   10,   10,   10,    0,  -10,
     0,   10,   20,   20,   20,   20,   10,    0,
     0,   10,   20,   40,   40,   20,   10,    0,
     0,   10,   20,   40,   40,   20,   10,    0,
     0,   10,   20,   20,   20,   20,   10,    0,
   -10,    0,   10,   10,   10,   10,    0,  -10,
   -50,  -10,    0,    0,    0,    0,  -10,  -50
};

const int KingOpening[64] = {
     0,    5,    5,  -10,  -10,    0,   10,    5,
   -30,  -30,  -30,  -30,  -30,  -30,  -30,  -30,
   -50,  -50,  -50,  -50,  -50,  -50,  -50,  -50,
   -70,  -70,  -70,  -70,  -70,  -70,  -70,  -70,
   -70,  -70,  -70,  -70,  -70,  -70,  -70,  -70,
   -70,  -70,  -70,  -70,  -70,  -70,  -70,  -70,
   -70,  -70,  -70,  -70,  -70,  -70,  -70,  -70,
   -70,  -70,  -70,  -70,  -70,  -70,  -70,  -70
};

// Mirror table for flipping piece-square tables for black pieces
const int Mirror64[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,   // rank 8 -> rank 1
    48, 49, 50, 51, 52, 53, 54, 55,   // rank 7 -> rank 2
    40, 41, 42, 43, 44, 45, 46, 47,   // rank 6 -> rank 3
    32, 33, 34, 35, 36, 37, 38, 39,   // rank 5 -> rank 4
    24, 25, 26, 27, 28, 29, 30, 31,   // rank 4 -> rank 5
    16, 17, 18, 19, 20, 21, 22, 23,   // rank 3 -> rank 6
     8,  9, 10, 11, 12, 13, 14, 15,   // rank 2 -> rank 7
     0,  1,  2,  3,  4,  5,  6,  7    // rank 1 -> rank 8
};

// Pawn structure masks (need to be implemented based on your bitboard system)
// These would need to be generated based on your specific bitboard implementation
extern uint64_t IsolatedMask[64];
extern uint64_t WhitePassedMask[64]; 
extern uint64_t BlackPassedMask[64];
extern uint64_t FileBBMask[8];

// Endgame material threshold
#define ENDGAME_MAT (1 * pieceVal[WR] + 2 * pieceVal[WN] + 2 * pieceVal[WP] + pieceVal[WK])

// Material draw detection function
int MaterialDraw(const s_board *pos) {
    ASSERT(CheckBoard(pos));
    
    // No major pieces (Rooks/Queens)
    if (!pos->piecenum[WR] && !pos->piecenum[BR] && !pos->piecenum[WQ] && !pos->piecenum[BQ]) {
        // No bishops
        if (!pos->piecenum[BB] && !pos->piecenum[WB]) {
            // Both sides have fewer than 3 knights
            if (pos->piecenum[WN] < 3 && pos->piecenum[BN] < 3) { 
                return TRUE; 
            }
        } 
        // No knights
        else if (!pos->piecenum[WN] && !pos->piecenum[BN]) {
            // Bishop count difference < 2
            if (abs(pos->piecenum[WB] - pos->piecenum[BB]) < 2) { 
                return TRUE; 
            }
        } 
        // Limited material combinations
        else if ((pos->piecenum[WN] < 3 && !pos->piecenum[WB]) || 
                 (pos->piecenum[WB] == 1 && !pos->piecenum[WN])) {
            if ((pos->piecenum[BN] < 3 && !pos->piecenum[BB]) || 
                (pos->piecenum[BB] == 1 && !pos->piecenum[BN])) { 
                return TRUE; 
            }
        }
    } 
    // Only rooks, no queens
    else if (!pos->piecenum[WQ] && !pos->piecenum[BQ]) {
        if (pos->piecenum[WR] == 1 && pos->piecenum[BR] == 1) {
            if ((pos->piecenum[WN] + pos->piecenum[WB]) < 2 && 
                (pos->piecenum[BN] + pos->piecenum[BB]) < 2) { 
                return TRUE; 
            }
        } else if (pos->piecenum[WR] == 1 && !pos->piecenum[BR]) {
            if ((pos->piecenum[WN] + pos->piecenum[WB] == 0) && 
                (((pos->piecenum[BN] + pos->piecenum[BB]) == 1) || 
                 ((pos->piecenum[BN] + pos->piecenum[BB]) == 2))) { 
                return TRUE; 
            }
        } else if (pos->piecenum[BR] == 1 && !pos->piecenum[WR]) {
            if ((pos->piecenum[BN] + pos->piecenum[BB] == 0) && 
                (((pos->piecenum[WN] + pos->piecenum[WB]) == 1) || 
                 ((pos->piecenum[WN] + pos->piecenum[WB]) == 2))) { 
                return TRUE; 
            }
        }
    }
    
    return FALSE;
}

// Main position evaluation function
int EvalPosition(s_board* pos) {
    ASSERT(CheckBoard(pos));

    int pce, pceNum, sq;
    int score = pos->material[WHITE] - pos->material[BLACK];
    
    // Check for draw by insufficient material
    if (!pos->piecenum[WP] && !pos->piecenum[BP] && MaterialDraw(pos) == TRUE) {
        return 0;
    }
    
    // Evaluate white pawns
    pce = WP;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        
        score += PawnTable[SQ64(sq)];
        
        if ((IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            score += PawnIsolated;
        }// Check for isolated pawns (requires IsolatedMask implementation)
 /* if ((WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) 
        {     // Check for passed pawns (requires PassedMask implementation)
    score += PawnPassed[RanksBrd[sq]];
        }*/
    }

    // Opening development bonuses and knight-rim penalties
    if (pos->hisply < 24) {
        // White development bonuses
        for (int i = 0; i < pos->piecenum[WN]; ++i) {
            int sq = pos->piecelist[WN][i];
            if (sq != B1 && sq != G1) score += DevMinorBonus; // developed
            if (FilesBrd[sq] == FILE_A || FilesBrd[sq] == FILE_H) score -= KnightRimPenalty;
        }
        for (int i = 0; i < pos->piecenum[WB]; ++i) {
            int sq = pos->piecelist[WB][i];
            if (sq != C1 && sq != F1) score += DevMinorBonus; // developed
        }

        // Black development bonuses (good for black -> subtract for white)
        for (int i = 0; i < pos->piecenum[BN]; ++i) {
            int sq = pos->piecelist[BN][i];
            if (sq != B8 && sq != G8) score -= DevMinorBonus; // developed
            if (FilesBrd[sq] == FILE_A || FilesBrd[sq] == FILE_H) score += KnightRimPenalty;
        }
        for (int i = 0; i < pos->piecenum[BB]; ++i) {
            int sq = pos->piecelist[BB][i];
            if (sq != C8 && sq != F8) score -= DevMinorBonus; // developed
        }
    }

    // Evaluate black pawns
    pce = BP;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        
        score -= PawnTable[Mirror64[SQ64(sq)]];
        if ((IsolatedMask[Mirror64[SQ64(sq)]] & pos->pawns[BLACK]) == 0) {
            score -= PawnIsolated;  // Negative for black
        }
        
        // Isolated and passed pawn evaluation for black would go here
    }
    
    // Evaluate white knights
    pce = WN;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += KnightTable[SQ64(sq)];
    }

    // Evaluate black knights
    pce = BN;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= KnightTable[Mirror64[SQ64(sq)]];
    }
    
    // Evaluate white bishops
    pce = WB;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += BishopTable[SQ64(sq)];
    }

    // Evaluate black bishops
    pce = BB;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= BishopTable[Mirror64[SQ64(sq)]];
    }

    // Evaluate white rooks
    pce = WR;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += RookTable[SQ64(sq)];
        
        ASSERT(FileRankValid(FilesBrd[sq]));
        
        // Open and semi-open file bonuses (requires FileBBMask implementation)
        // if (!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
        //     score += RookOpenFile;
        // } else if (!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
        //     score += RookSemiOpenFile;
        // }
    }

    // Evaluate black rooks
    pce = BR;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= RookTable[Mirror64[SQ64(sq)]];
        
        // Open and semi-open file evaluation for black would go here
    }
    
    // Evaluate white queens
    pce = WQ;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += QueenTable[SQ64(sq)];
        
        // Queen open/semi-open file evaluation would go here
    }

    // Evaluate black queens
    pce = BQ;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= QueenTable[Mirror64[SQ64(sq)]];
        
        // Queen open/semi-open file evaluation would go here
    }
    
    // Evaluate white king
    pce = WK;
    sq = pos->piecelist[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
    
    if (pos->material[BLACK] <= ENDGAME_MAT) {
        score += KingEndgame[SQ64(sq)];
    } else {
        score += KingOpening[SQ64(sq)];
    }
    
    // Evaluate black king
    pce = BK;
    sq = pos->piecelist[pce][0];
    ASSERT(SqOnBoard(sq));
    ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
    
    if (pos->material[WHITE] <= ENDGAME_MAT) {
        score -= KingEndgame[Mirror64[SQ64(sq)]];
    } else {
        score -= KingOpening[Mirror64[SQ64(sq)]];
    }
    
    // Bishop pair bonus
    if (pos->piecenum[WB] >= 2) score += BishopPair;
    if (pos->piecenum[BB] >= 2) score -= BishopPair;
    
    // Castling bonus
    if (pos->king[WHITE] == 27 || pos->king[WHITE] == 23) score += CastleBonus;
    if (pos->king[BLACK] == 97 || pos->king[BLACK] == 93) score -= CastleBonus;

    // Uncastled penalty (opening only): king still on E1/E8 and castling rights exist
    if (pos->hisply < 30) {
        if (pos->king[WHITE] == E1 && (pos->castleperm & (WKCA | WQCA))) {
            score -= UncastledPenalty;
        }
        if (pos->king[BLACK] == E8 && (pos->castleperm & (BKCA | BQCA))) {
            score += UncastledPenalty;
        }
        // Early queen development penalty if queen left starting square
        if (pos->piecenum[WQ] == 1) {
            int wqSq = pos->piecelist[WQ][0];
            if (wqSq != D1) score -= EarlyQueenPenalty;
        }
        if (pos->piecenum[BQ] == 1) {
            int bqSq = pos->piecelist[BQ][0];
            if (bqSq != D8) score += EarlyQueenPenalty;
        }
    }

    // Hanging piece penalties: piece is attacked by opponent and not defended by own side
    // White pieces hanging -> bad for white (subtract). Black hanging -> good for white (add)
    {
        // White side
        for (int pce = WN; pce <= WQ; ++pce) {
            for (int i = 0; i < pos->piecenum[pce]; ++i) {
                int sq = pos->piecelist[pce][i];
                if (SqAttacked(sq, BLACK, pos) && !SqAttacked(sq, WHITE, pos)) {
                    int pen = HangingBasePenalty + pieceVal[pce] / 3;
                    score -= pen;
                }
            }
        }
        // Black side
        for (int pce = BN; pce <= BQ; ++pce) {
            for (int i = 0; i < pos->piecenum[pce]; ++i) {
                int sq = pos->piecelist[pce][i];
                if (SqAttacked(sq, WHITE, pos) && !SqAttacked(sq, BLACK, pos)) {
                    int pen = HangingBasePenalty + pieceVal[pce] / 3;
                    score += pen;
                }
            }
        }
    }

    // Return score from the perspective of the side to move
    if (pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}
