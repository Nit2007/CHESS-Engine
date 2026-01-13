#include "defs.h"
#include "struct.h"

void InitMvvLva() {
    // Initialize Most Valuable Victim - Least Valuable Attacker scores
    // Higher score = better capture (capture valuable piece with less valuable piece)
    
    for (int attacker = WP; attacker <= BK; ++attacker) {
        for (int victim = WP; victim <= BK; ++victim) {
            // Score = (victim value * 100) + (100 - attacker value/100)
            // This prioritizes capturing valuable pieces and using less valuable attackers
            MvvLvaScores[victim][attacker] =
                MvvLvaValues[victim] * 10 - MvvLvaValues[attacker];
        }   
    }
}

int ScoreMove(const int move, const s_board* pos) {
    int score = 0;
    // Check if this is the PV move (from hash table)
    int pvMove = ProbeHashMove(pos);
    if (move == pvMove) {
        score += PV_MOVE_SCORE;
    }
    
    // Castling bonus
    if (move & MFLAGCA) {
        score += 2000;
    }
    // Penalize early queen development for quiet moves in opening to prevent pawn-hunting
    {
        int from = FROMSQ(move);
        int attacker = pos->pieces[from];
        if ((attacker == WQ || attacker == BQ) && CAPTURED(move) == EMPTY) {
            if (pos->hisply < 20) {
                score -= 3000; // reduce priority of quiet queen moves in opening
            }
        }
    }

    // Opening: prefer simple space-grabbing central pawn pushes
    {
        if (pos->hisply < 12 && CAPTURED(move) == EMPTY) {
            int from = FROMSQ(move);
            int to = TOSQ(move);
            int pce = pos->pieces[from];
            if (pce == WP || pce == BP) {
                if (to == C4 || to == D4 || to == E4 || to == C5 || to == D5 || to == E5) {
                    score += 2500;
                }
            }
        }
    }

    int captured = CAPTURED(move);
    
    if (captured != EMPTY) {
        // This is a capture move - use MVV-LVA scoring
        int attacker = pos->pieces[FROMSQ(move)];
        if (attacker >= WP && attacker <= BK) {
            ASSERT(PieceValid(attacker));
            ASSERT(PieceValid(captured));
            
            score = MvvLvaScores[captured][attacker] + 100000; // Base capture score

            // Opening: discourage early knight-for-pawn grabs (esp. deep pawn raids)
            if (pos->hisply < 16 && (attacker == WN || attacker == BN) && (captured == WP || captured == BP)) {
                int to = TOSQ(move);
                if (RanksBrd[to] == RANK_1 || RanksBrd[to] == RANK_2 || RanksBrd[to] == RANK_7 || RanksBrd[to] == RANK_8) {
                    score -= 8000;
                }
            }
        }
    } else {
        // Non-capture moves

        // Opening: discourage weird knight maneuvers (rim / deep raids) so development/space is searched first
        {
            if (pos->hisply < 12) {
                int from = FROMSQ(move);
                int to = TOSQ(move);
                int pce = pos->pieces[from];
                if (pce == WN || pce == BN) {
                    if (FilesBrd[to] == FILE_A || FilesBrd[to] == FILE_H) score -= 2000;
                    if (RanksBrd[to] == RANK_1 || RanksBrd[to] == RANK_2 || RanksBrd[to] == RANK_7 || RanksBrd[to] == RANK_8) score -= 2500;
                }
            }
        }
        
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
    int bestScore = list->moves[moveNum].score;
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
