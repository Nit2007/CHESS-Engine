#include "defs.h"
#include "struct.h"

 
// Time/stop checker used during search
static inline void CheckUp(s_searchinfo* info) {
    // Stop if the GUI asked to quit or stop
    if (info->quit) {
        info->stopped = TRUE;
        return;
    }
    // Stop if we are time-limited and time is over
    if (info->timeset) {
        if (GetTimeMs() > info->stoptime) {
            info->stopped = TRUE;
        }
    }
}
int IsRepetition(s_board* pos)
{
    // Ensure valid bounds for history scan
    int start = pos->hisply - pos->fifty;
    if (start < 0) start = 0;
    int end = pos->hisply - 1;
    if (end < 0) return FALSE;

    for (int index = start; index < end; index++) {
        if (index >= 0 && index < MAXGAMEMOVES) {
            if (pos->poskey == pos->history[index].poskey) return TRUE;
        } else {
            break;
        }
    }
    return FALSE;
}

void ClearForSearch(s_board* pos, s_searchinfo* info) {
    // Clear history heuristic table
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 120; j++) {
            pos->searchHistory[i][j] = 0;
        }
    }

    // Clear killer moves
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            pos->searchKillers[i][j] = 0;
        }
    }

    // Reset hash table stats
    ClearHashTable(pos->hashtable);
    pos->ply = 0;
    pos->hashtable->overWrite = 0;
    pos->hashtable->hit = 0;
    pos->hashtable->cut = 0;

    // Set timing information
    info->starttime = GetTimeMs();
    
    // ❗ DO NOT reset stoptime — UCI already set the time budget
    // info->stoptime = 0;  <-- REMOVED to avoid wiping UCI's time budget

    // Reset other search stats
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
    info->bestmove = 0;
    info->bestscore = 0;
}


 int AlphaBeta(int alpha, int beta, int depth, s_board* pos, s_searchinfo* info, int DoNULL) {
    ASSERT(CheckBoard(pos));
    
    // Periodically check for time/stop
    // Periodically check for time/stop
if ((info->nodes & 2047) == 0) {
    CheckUp(info);
}
if (info->stopped) {
    return 0;
}

    if (depth == 0) {
        info->nodes++;
        return EvalPosition(pos);
    }
    
    info->nodes++;
    
    if (IsRepetition(pos) || pos->fifty == 100) return 0;
    if (pos->ply > MAXDEPTH - 1) return EvalPosition(pos);
    
    int oldalpha = alpha;
    int bestmove = 0;
    int score = -INFINITE;
    int legal = 0;
    
    s_movelist list;
    GenerateAllMoves(pos, &list);
    // Probe hash table for best move
int hashMove = ProbeHashMove(pos);
if (hashMove != 0) {
    // Move hash move to front of list for better ordering
    for (int i = 0; i < list.count; i++) {
        if (list.moves[i].move == hashMove) {
            // Swap with first move
            int tempMove = list.moves[0].move;
            int tempScore = list.moves[0].score;
            list.moves[0].move = list.moves[i].move;
            list.moves[0].score = list.moves[i].score;
            list.moves[i].move = tempMove;
            list.moves[i].score = tempScore;
            break;
        }
    }
}
    // Sort moves for better ordering
    for (int movenum = 0; movenum < list.count; movenum++) {
        PickNextMove(movenum, &list);
        
        if (!MakeMove(pos, list.moves[movenum].move)) continue;
        
        legal++;
        score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
        TakeMove(pos);
        
        if (info->stopped) {
            return 0;
        }
        
        if (score > alpha) {
            if (score >= beta) {
                if (legal == 1) info->fhf++;
                info->fh++;
                
                // Update killer moves for non-capture moves
                if (CAPTURED(list.moves[movenum].move) == EMPTY) {
                    // Shift killer moves
                    pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                    pos->searchKillers[0][pos->ply] = list.moves[movenum].move;
                }
                
                return beta;
            }
            alpha = score;
            bestmove = list.moves[movenum].move;
            
            // Update history heuristic for non-capture moves
            if (CAPTURED(bestmove) == EMPTY) {
                int from = FROMSQ(bestmove);
                int to = TOSQ(bestmove);
                int piece = pos->pieces[from];
                
                ASSERT(SqOnBoard(from) && SqOnBoard(to));
                if (piece >= WP && piece <= BK) {
                    ASSERT(PieceValid(piece));
                    
                    pos->searchHistory[piece][to] += depth * depth; // Depth squared bonus
                }
            }
        }
    }
    
    if (legal == 0) {
        if (SqAttacked(pos->king[pos->side], pos->side^1, pos)) {
            return -ISMATE + pos->ply;
        } else {
            return 0;
        }
    }
    
    if (oldalpha != alpha) {
        StoreHashMove(pos, bestmove);
    }
    
    return alpha;
}

int Quiescence(int alpha, int beta, s_board* pos, s_searchinfo* info)
 {//To avoid Horizon affect of AlphaBeta (due to depth constraints)
    ASSERT(CheckBoard(pos));
    
    info->nodes++;
    // Periodically check for time/stop
    if ((info->nodes & 2047) == 0) {
        CheckUp(info);
    }
    if (info->stopped) {
        return 0;
    }
    
    // Check for repetition or fifty-move rule
    if (IsRepetition(pos) || pos->fifty >= 100) return 0;
    
    // Prevent search explosion at extreme depths
    if (pos->ply > MAXDEPTH - 1) return EvalPosition(pos);
    
    // Stand pat: evaluate current position
    // In quiescence, we assume the current position is "good enough"
    int standPat = EvalPosition(pos);
    
    // Beta cutoff on stand pat score
    if (standPat >= beta) {
        return beta;
    }
    
    // Alpha improvement
    if (standPat > alpha) {
        alpha = standPat;
    }
    
    // Generate only capture moves in quiescence search
    s_movelist list;
    list.count = 0;
    
    // Generate captures only (modify GenerateAllMoves or create a capture-only version)
    GenerateAllCaptures(pos, &list);  // You'll need to implement this
    
    // If no captures available, return stand pat score
    if (list.count == 0) {
        return standPat;
    }
    
    int legal = 0;
    int bestscore = standPat;
    // Probe hash table for best move
int hashMove = ProbeHashMove(pos);
if (hashMove != 0) {
    // Move hash move to front of list for better ordering
    for (int i = 0; i < list.count; i++) {
        if (list.moves[i].move == hashMove) {
            // Swap with first move
            int tempMove = list.moves[0].move;
            int tempScore = list.moves[0].score;
            list.moves[0].move = list.moves[i].move;
            list.moves[0].score = list.moves[i].score;
            list.moves[i].move = tempMove;
            list.moves[i].score = tempScore;
            break;
        }
    }
}
    // Try all capture moves
    for (int movenum = 0; movenum < list.count; movenum++) {
        PickNextMove(movenum, &list);
        
        if (!MakeMove(pos, list.moves[movenum].move)) continue;
        
        legal++;
        int score = -Quiescence(-beta, -alpha, pos, info);
        TakeMove(pos);
        
        if (score > bestscore) {
            bestscore = score;
            
            if (score > alpha) {
                if (score >= beta) {
                    return beta;  // Beta cutoff
                }
                alpha = score;
            }
        }
    }
    
    return bestscore;
}

void SearchPosition(s_board* pos, s_searchinfo* info)
{//Iterative Deepening
    int bestmove = 0;
    int bestscore= -INFINITE;
    int pvmove=0;
    
    ClearForSearch(pos,  info);
    
    for(int depth=1; depth<=info->depth ;depth++)
    {
        if (info->stopped) {
            break;
        }
        bestscore = AlphaBeta( -INFINITE , INFINITE , depth, pos, info, true);
        pvmove = GetHashLine( depth, pos);
        bestmove = pos->pvarray[0];
        
        // Store best move in search info for UCI
        info->bestmove = bestmove;
        info->bestscore = bestscore;
    }

    // Fallback: ensure we always have a legal move to play
    if (info->bestmove == 0) {
        s_movelist list;
        GenerateAllMoves(pos, &list);
        if (list.count > 0) {
            info->bestmove = list.moves[0].move;
        }
    }
}


