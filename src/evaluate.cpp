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
const int DevMinorBonus = 18;         // bonus for minor pieces developed in opening
const int KnightRimPenalty = 22;      // penalty for knights on rim in opening
const int KnightEarlyRaidPenalty = 120;
const int OpeningSpaceBonus = 12;
const int OpeningHangingMinorExtra = 90;

// Piece-square tables (from white's perspective)
const int PawnTable[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,          // Rank 8 (Promotion - handled by move gen/eval)
   50, 50, 50, 50, 50, 50, 50, 50,          // Rank 7 (Strong, but not worth a whole piece yet)
   10, 10, 20, 30, 30, 20, 10, 10,          // Rank 6
    5,  5, 10, 25, 25, 10,  5,  5,          // Rank 5
    0,  0,  0, 20, 20,  0,  0,  0,          // Rank 4 (Center control)
    5, -5,-10,  0,  0,-10, -5,  5,          // Rank 3 (Avoid blocking bishops)
    5, 10, 10,-20,-20, 10, 10,  5,          // Rank 2 (Defensive base)
    0,  0,  0,  0,  0,  0,  0,  0           // Rank 1
};

const int PawnTableEndgame[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,          // Rank 8 (Promotion)
    150, 150, 150, 150, 150, 150, 150, 150, // Rank 7 (Massive push incentive)
    100, 110, 120, 130, 130, 120, 110, 100, // Rank 6
     60,  70,  80,  90,  90,  80,  70,  60, // Rank 5
     30,  40,  50,  60,  60,  50,  40,  30, // Rank 4
     10,  20,  25,  30,  30,  25,  20,  10, // Rank 3
      0,   0,   0,   0,   0,   0,   0,   0, // Rank 2
      0,   0,   0,   0,   0,   0,   0,   0  // Rank 1
};

const int KnightTable[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,        // Rank 8
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50         // Rank 1
};

const int BishopTable[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int RookTable[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,         // Rank 8
    15, 20, 20, 20, 20, 20, 20, 15,         // Rank 7 (The "Pig" on the 7th)
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0          // Rank 1
};

const int QueenTable[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
      0,  0,  5,  5,  5,  5,  0, -5,
     -5,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

// King tables for endgame and opening/middlegame
const int KingOpening[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,        // Rank 8
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,        // Rank 2
     20, 30, 10,  0,  0, 10, 30, 20         // Rank 1 (C1 and G1 are gold)
};

const int KingEndgame[64] = {
    -50,-30,-30,-30,-30,-30,-30,-50,
    -30,-10,  0,  0,  0,  0,-10,-30,
    -30,  0, 20, 30, 30, 20,  0,-30,
    -30,  0, 30, 40, 40, 30,  0,-30,
    -30,  0, 30, 40, 40, 30,  0,-30,
    -30,  0, 20, 30, 30, 20,  0,-30,
    -30,-10,  0,  0,  0,  0,-10,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
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

static inline bool IsWhitePassedPawn(const s_board* pos, int sq) {
    const int file = FilesBrd[sq];
    const int rank = RanksBrd[sq];

    for (int f = file - 1; f <= file + 1; ++f) {
        if (f < FILE_A || f > FILE_H) continue;
        for (int r = rank + 1; r <= RANK_8; ++r) {
            const int tSq = smalltobig(f, r);
            if (pos->pieces[tSq] == BP) return false;
        }
    }
    return true;
}

static inline bool IsBlackPassedPawn(const s_board* pos, int sq) {
    const int file = FilesBrd[sq];
    const int rank = RanksBrd[sq];

    for (int f = file - 1; f <= file + 1; ++f) {
        if (f < FILE_A || f > FILE_H) continue;
        for (int r = rank - 1; r >= RANK_1; --r) {
            const int tSq = smalltobig(f, r);
            if (pos->pieces[tSq] == WP) return false;
        }
    }
    return true;
}

// Threshold: 50,000 (King) + 1,300 (Active Material)
const int ENDGAME_MAT = 51300; 

bool IsEndgame(s_board *pos) {
    // Check White: No Queen and material below threshold
    if (pos->piecenum[WQ] == 0 && pos->material[WHITE] <= ENDGAME_MAT) {
        return true;
    }
    // Check Black: No Queen and material below threshold
    if (pos->piecenum[BQ] == 0 && pos->material[BLACK] <= ENDGAME_MAT) {
        return true;
    }
    
    // Optional: If both sides are very low on material even with Queens
    if (pos->material[WHITE] <= ENDGAME_MAT && pos->material[BLACK] <= ENDGAME_MAT) {
        return true;
    }

    return false;
}

int EvalPosition(s_board* pos) {
    ASSERT(CheckBoard(pos));
    
    int pce, pceNum, sq;
    int score = pos->material[WHITE] - pos->material[BLACK];
    bool endgame = IsEndgame(pos);
    // Evaluate white pieces with piece-square tables
    pce = WP;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        if (endgame) {
            score += PawnTableEndgame[SQ64(sq)];
        } else {
            score += PawnTable[SQ64(sq)];
        }
    }
    
    
    pce = WN;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += KnightTable[SQ64(sq)];
    }
    
    pce = WB;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += BishopTable[SQ64(sq)];
    }
    
    pce = WR;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += RookTable[SQ64(sq)];
    }
    
    pce = WQ;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        score += QueenTable[SQ64(sq)];
    }
    
    // Evaluate black pieces with piece-square tables (mirrored)
    pce = BP;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        if (endgame) {
            score -= PawnTableEndgame[Mirror64[SQ64(sq)]];
        } else {
            score -= PawnTable[Mirror64[SQ64(sq)]];
        }
    }
    
    pce = BN;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= KnightTable[Mirror64[SQ64(sq)]];
    }
    
    pce = BB;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= BishopTable[Mirror64[SQ64(sq)]];
    }
    
    pce = BR;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= RookTable[Mirror64[SQ64(sq)]];
    }
    
    pce = BQ;
    for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
        sq = pos->piecelist[pce][pceNum];
        ASSERT(SqOnBoard(sq));
        ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        score -= QueenTable[Mirror64[SQ64(sq)]];
    }
    
    
    int whiteKingSq = pos->king[WHITE];
    int blackKingSq = pos->king[BLACK];

    // Ensure squares are valid for index mapping
    ASSERT(SqOnBoard(whiteKingSq));
    ASSERT(SqOnBoard(blackKingSq));

    if (endgame) {
        score += KingEndgame[SQ64(whiteKingSq)];
    } else {
        score += KingOpening[SQ64(whiteKingSq)];
    }

    if (endgame) {
        score -= KingEndgame[Mirror64[SQ64(blackKingSq)]];
    } else {
        score -= KingOpening[Mirror64[SQ64(blackKingSq)]];
    }


    // Return score from perspective of side to move
    if (pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}
// Main position evaluation function
// int EvalPosition(s_board* pos) {
//     ASSERT(CheckBoard(pos));

//     int pce, pceNum, sq;
//     int score = pos->material[WHITE] - pos->material[BLACK];

//     const bool endgameOnly = (pos->material[WHITE] <= ENDGAME_MAT && pos->material[BLACK] <= ENDGAME_MAT);
    
//     // Check for draw by insufficient material
//     if (!pos->piecenum[WP] && !pos->piecenum[BP] && MaterialDraw(pos) == TRUE) {
//         return 0;
//     }
    
//     // Evaluate white pawns
//     pce = WP;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
        
//         score += PawnTable[SQ64(sq)];

//         if (pos->hisply < 16) {
//             if (sq == C4 || sq == D4 || sq == E4 || sq == C5 || sq == D5 || sq == E5) {
//                 score += OpeningSpaceBonus;
//             }
//         }

//         if (endgameOnly && IsWhitePassedPawn(pos, sq)) {
//             score += PawnPassed[RanksBrd[sq]];
//         }
        
//         // if ((IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
//         //     score += PawnIsolated;
//         // }// Check for isolated pawns (requires IsolatedMask implementation)
//  /* if ((WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) 
//         {     // Check for passed pawns (requires PassedMask implementation)
//     score += PawnPassed[RanksBrd[sq]];
//         }*/
//     }

//     // Opening development bonuses and knight-rim penalties
//     if (pos->hisply < 24) {
//         // White development bonuses
//         for (int i = 0; i < pos->piecenum[WN]; ++i) {
//             int sq = pos->piecelist[WN][i];
//             if (sq != B1 && sq != G1) score += DevMinorBonus; // developed
//             if (FilesBrd[sq] == FILE_A || FilesBrd[sq] == FILE_H) score -= KnightRimPenalty;
//             if (pos->hisply < 16 && (RanksBrd[sq] == RANK_7 || RanksBrd[sq] == RANK_8)) score -= KnightEarlyRaidPenalty;
//         }
//         for (int i = 0; i < pos->piecenum[WB]; ++i) {
//             int sq = pos->piecelist[WB][i];
//             if (sq != C1 && sq != F1) score += DevMinorBonus; // developed
//         }

//         // Black development bonuses (good for black -> subtract for white)
//         for (int i = 0; i < pos->piecenum[BN]; ++i) {
//             int sq = pos->piecelist[BN][i];
//             if (sq != B8 && sq != G8) score -= DevMinorBonus; // developed
//             if (FilesBrd[sq] == FILE_A || FilesBrd[sq] == FILE_H) score += KnightRimPenalty;
//             if (pos->hisply < 16 && (RanksBrd[sq] == RANK_1 || RanksBrd[sq] == RANK_2)) score += KnightEarlyRaidPenalty;
//         }
//         for (int i = 0; i < pos->piecenum[BB]; ++i) {
//             int sq = pos->piecelist[BB][i];
//             if (sq != C8 && sq != F8) score -= DevMinorBonus; // developed
//         }
//     }

//     if (pos->hisply < 30) {
//         if (pos->king[WHITE] == E1 && (pos->castleperm & (WKCA | WQCA))) {
//             score -= UncastledPenalty;
//         }
//         if (pos->king[BLACK] == E8 && (pos->castleperm & (BKCA | BQCA))) {
//             score += UncastledPenalty;
//         }
 
//         if (pos->piecenum[WQ] == 1) {
//             int wqSq = pos->piecelist[WQ][0];
//             if (wqSq != D1) score -= EarlyQueenPenalty;
//         }
//         if (pos->piecenum[BQ] == 1) {
//             int bqSq = pos->piecelist[BQ][0];
//             if (bqSq != D8) score += EarlyQueenPenalty;
//         }
//     }

//     // Evaluate black pawns
//     pce = BP;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
        
//         score -= PawnTable[Mirror64[SQ64(sq)]];

//         if (pos->hisply < 16) {
//             if (sq == C4 || sq == D4 || sq == E4 || sq == C5 || sq == D5 || sq == E5) {
//                 score -= OpeningSpaceBonus;
//             }
//         }

//         if (endgameOnly && IsBlackPassedPawn(pos, sq)) {
//             score -= PawnPassed[7 - RanksBrd[sq]];
//         }
//         // if ((IsolatedMask[Mirror64[SQ64(sq)]] & pos->pawns[BLACK]) == 0) {
//         //     score -= PawnIsolated;  // Negative for black
//         // }
        
//         // Isolated and passed pawn evaluation for black would go here
//     }
    
//     // Evaluate white knights
//     pce = WN;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
//         score += KnightTable[SQ64(sq)];
//     }

//     // Evaluate black knights
//     pce = BN;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
//         score -= KnightTable[Mirror64[SQ64(sq)]];
//     }
    
//     // Evaluate white bishops
//     pce = WB;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
//         score += BishopTable[SQ64(sq)];
//     }

//     // Evaluate black bishops
//     pce = BB;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
//         score -= BishopTable[Mirror64[SQ64(sq)]];
//     }

//     // Evaluate white rooks
//     pce = WR;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
//         score += RookTable[SQ64(sq)];
        
//         ASSERT(FileRankValid(FilesBrd[sq]));
        
//         // Open and semi-open file bonuses (requires FileBBMask implementation)
//         // if (!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
//         //     score += RookOpenFile;
//         // } else if (!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
//         //     score += RookSemiOpenFile;
//         // }
//     }

//     // Evaluate black rooks
//     pce = BR;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
//         score -= RookTable[Mirror64[SQ64(sq)]];
        
//         // Open and semi-open file evaluation for black would go here
//     }
    
//     // Evaluate white queens
//     pce = WQ;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
//         score += QueenTable[SQ64(sq)];
        
//         // Queen open/semi-open file evaluation would go here
//     }

//     // Evaluate black queens
//     pce = BQ;
//     for (pceNum = 0; pceNum < pos->piecenum[pce]; ++pceNum) {
//         sq = pos->piecelist[pce][pceNum];
//         ASSERT(SqOnBoard(sq));
//         ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
//         score -= QueenTable[Mirror64[SQ64(sq)]];
        
//         // Queen open/semi-open file evaluation would go here
//     }
    
//     // Evaluate white king
//     pce = WK;
//     sq = pos->piecelist[pce][0];
//     ASSERT(SqOnBoard(sq));
//     ASSERT(SQ64(sq) >= 0 && SQ64(sq) <= 63);
    
//     if (pos->material[BLACK] <= ENDGAME_MAT) {
//         score += KingEndgame[SQ64(sq)];
//     } else {
//         score += KingOpening[SQ64(sq)];
//     }
    
//     // Evaluate black king
//     pce = BK;
//     sq = pos->piecelist[pce][0];
//     ASSERT(SqOnBoard(sq));
//     ASSERT(Mirror64[SQ64(sq)] >= 0 && Mirror64[SQ64(sq)] <= 63);
    
//     if (pos->material[WHITE] <= ENDGAME_MAT) {
//         score -= KingEndgame[Mirror64[SQ64(sq)]];
//     } else {
//         score -= KingOpening[Mirror64[SQ64(sq)]];
//     }
    
//     // Bishop pair bonus
//     if (pos->piecenum[WB] >= 2) score += BishopPair;
//     if (pos->piecenum[BB] >= 2) score -= BishopPair;
    
//    // White
// if (!(pos->castleperm & WKCA) && pos->king[WHITE] != E1 && FilesBrd[pos->king[WHITE]] >= FILE_F) {
//     score += CastleBonus;
// }
// if (!(pos->castleperm & WQCA) && pos->king[WHITE] != E1 && FilesBrd[pos->king[WHITE]] <= FILE_C) {
//     score += CastleBonus;
// }

// // Black
// if (!(pos->castleperm & BKCA) && pos->king[BLACK] != E8 && FilesBrd[pos->king[BLACK]] >= FILE_F) {
//     score -= CastleBonus;
// }
// if (!(pos->castleperm & BQCA) && pos->king[BLACK] != E8 && FilesBrd[pos->king[BLACK]] <= FILE_C) {
//     score -= CastleBonus;
// }
//     // // Uncastled penalty (opening only): king still on E1/E8 and castling rights exist
//     // if (pos->hisply < 30) {
//     //     if (pos->king[WHITE] == E1 && (pos->castleperm & (WKCA | WQCA))) {
//     //         score -= UncastledPenalty;
//     //     }
//     //     if (pos->king[BLACK] == E8 && (pos->castleperm & (BKCA | BQCA))) {
//     //         score += UncastledPenalty;
//     //     }
//     //     // Early queen development penalty if queen left starting square
//     //     if (pos->piecenum[WQ] == 1) {
//     //         int wqSq = pos->piecelist[WQ][0];
//     //         if (wqSq != D1) score -= EarlyQueenPenalty;
//     //     }
//     //     if (pos->piecenum[BQ] == 1) {
//     //         int bqSq = pos->piecelist[BQ][0];
//     //         if (bqSq != D8) score += EarlyQueenPenalty;
//     //     }
//     // }

//     // Hanging piece penalties: piece is attacked by opponent and not defended by own side
//     // White pieces hanging -> bad for white (subtract). Black hanging -> good for white (add)
//     {
//         // White side
//         for (int pce = WN; pce <= WQ; ++pce) {
//             for (int i = 0; i < pos->piecenum[pce]; ++i) {
//                 int sq = pos->piecelist[pce][i];
//                 if (SqAttacked(sq, BLACK, pos) && !SqAttacked(sq, WHITE, pos)) {
//                     int pen = HangingBasePenalty + pieceVal[pce] / 3;
//                     if (pos->hisply < 20 && (pce == WN || pce == WB)) {
//                         pen += OpeningHangingMinorExtra;
//                     }
//                     score -= pen;
//                 }
//             }
//         }
//         // Black side
//         for (int pce = BN; pce <= BQ; ++pce) {
//             for (int i = 0; i < pos->piecenum[pce]; ++i) {
//                 int sq = pos->piecelist[pce][i];
//                 if (SqAttacked(sq, WHITE, pos) && !SqAttacked(sq, BLACK, pos)) {
//                     int pen = HangingBasePenalty + pieceVal[pce] / 3;
//                     if (pos->hisply < 20 && (pce == BN || pce == BB)) {
//                         pen += OpeningHangingMinorExtra;
//                     }
//                     score += pen;
//                 }
//             }
//         }
//     }

//     // Return score from the perspective of the side to move
//     if (pos->side == WHITE) {
//         return score;
//     } else {
//         return -score;
//     }
// }
