#include "defs.h"
#include "struct.h"

static void CheckUp()
{//to check if time is up to interupt GUI
    
}
int IsRepetition(s_board* pos)
{
    for(int index=pos->hisply-pos->fifty;index<pos->hisply-1;index++)
      {    ASSERT(index>=0 && index <2048);
          if(pos->poskey == pos->history[index].poskey)return TRUE;
      }
  return FALSE;
}

static void ClearForSearch(s_board* pos, s_searchinfo* info) {
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
    
    ClearHashTable(pos->hashtable);
    pos->ply = 0;
    
    pos->hashtable->overWrite = 0;
    pos->hashtable->hit = 0;
    pos->hashtable->cut = 0;
    
    info->starttime = GetTimeMs();
    info->stoptime = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}


static int AlphaBeta(int alpha, int beta, int depth, s_board* pos, s_searchinfo* info, int DoNULL) {
    ASSERT(CheckBoard(pos));
    
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
    
    // Sort moves for better ordering
    for (int movenum = 0; movenum < list.count; movenum++) {
        PickNextMove(movenum, &list);
        
        if (!MakeMove(pos, list.moves[movenum].move)) continue;
        
        legal++;
        score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
        TakeMove(pos);
        
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
                ASSERT(PieceValid(piece));
                
                pos->searchHistory[piece][to] += depth * depth; // Depth squared bonus
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

static int Quiescence(int alpha,int beta,int depth ,s_board* pos, s_searchinfo* info)
{//To avoid Horizon affect of AlphaBeta (due to depth constraints)
    return 0;
}

void SearchPosition(s_board* pos, s_searchinfo* info)
{//Iterative Deepening
	int bestmove = 0;
	int bestscore= -INFINITE;
	int pvmove=0;
	
	ClearForSearch(pos,  info);
	
	for(int depth=1; depth<=info->depth ;depth++)
	{
		bestscore = AlphaBeta( -INFINITE , INFINITE , depth, pos, info, true);
		pvmove = GetHashLine( depth, pos);
		bestmove = pos->pvarray[0]; 
		cout<<"At Depth : "<<depth<<"With BestScore : "<<bestscore<<" BestMove : "<<PrMove(bestmove)<<" Nodes Visited : "<<info->nodes<<endl;
		
		pvmove = GetHashLine( depth, pos);
		cout<<" Principal Variation (Hash-Table) ";
		for(int pvnum=0;pvnum<pvmove;pvnum++)
		{
			cout<<" "<<PrMove(bestmove);
		}cout<<endl;
		cout<<"Ordering : "<<fixed<<setprecision(2)<<(info->fh > 0 ? info->fhf/info->fh : 0.0);
	}
}

