#include "defs.h"
#include "struct.h"

void InitMvvLva() {
    // Initialize Most Valuable Victim - Least Valuable Attacker scores
    // Higher score = better capture (capture valuable piece with less valuable piece)
    
    for (int attacker = WP; attacker <= BK; ++attacker) {
        for (int victim = WP; victim <= BK; ++victim) {
            // Score = (victim value * 100) + (100 - attacker value/100)
            // This prioritizes capturing valuable pieces and using less valuable attackers
            MvvLvaScores[victim][attacker] = MvvLvaValues[victim] + 6 - (MvvLvaValues[attacker] / 100);
        }   
    }
}

int ScoreMove(const int move, const s_board* pos) {
    int score = 0;
    // Check if this is the PV move (from hash table)
    int pvMove = ProbeHashMove(pos);
    if (move == pvMove) {
        return PV_MOVE_SCORE;
    }
    
    int captured = CAPTURED(move);
    
    if (captured != EMPTY) {
        // This is a capture move - use MVV-LVA scoring
        int attacker = pos->pieces[FROMSQ(move)];
        if (attacker >= WP && attacker <= BK) {
            ASSERT(PieceValid(attacker));
            ASSERT(PieceValid(captured));
            
            score = MvvLvaScores[captured][attacker] + 100000; // Base capture score
        }
    } else {
        // Non-capture moves
        
        // Check if this move is a killer move
        if (pos->searchKillers[0][pos->ply] == move) {
            score = KILLER_ONE_SCORE;
        } else if (pos->searchKillers[1][pos->ply] == move) {
            score = KILLER_TWO_SCORE;
        } else {
            // Use history heuristic score
            int from = FROMSQ(move);
            int to = TOSQ(move);
            int piece = pos->pieces[from];
            
            ASSERT(SqOnBoard(from));
            ASSERT(SqOnBoard(to));
            if (piece >= WP && piece <= BK) {
                ASSERT(PieceValid(piece));
                
                score = pos->searchHistory[piece][to];
            }
        }
    }
    
    return score;
}

void PickNextMove(int moveNum, s_movelist* list) {
    // Find the move with the highest score from moveNum onwards and swap it to position moveNum
    // This implements a simple selection sort for move ordering
    int bestScore = 0;
    int bestNum = moveNum;
    
    for (int index = moveNum; index < list->count; ++index) {
        if (list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    
    // Swap the best move to the current position
    if (bestNum != moveNum) {
        s_move temp = list->moves[moveNum];
        list->moves[moveNum] = list->moves[bestNum];
        list->moves[bestNum] = temp;
    }
}
