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

static void ClearForSearch(s_board* pos, s_searchinfo* info)
{
    for(int i=0;i<13;i++)
        {
            for(int j=0;j<120;j++)
            {
                pos->searchHistory[i][j]=0;
            }
        }
    for(int i=0;i<2;i++)
        {
            for(int j=0;j<64;j++)
            {
                pos->searchKillers[i][j]=0;
            }
        }
    ClearHashTable(pos->hashtable);
    pos->ply=0;//Make the current ply search to zero ,not hisply which represents the total no of half moves made

    pos->hashtable->overWrite=0;
	pos->hashtable->hit=0;
	pos->hashtable->cut=0;

	info->starttime = GetTimeMs();
	info->stoptime = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}


static int AlphaBeta(int alpha,int beta,s_board* pos, s_searchinfo* info,,int DoNULL)
{
    return 0;
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
		bestscore = AlphaBeta( -INFINITE , INFINITE , pos, info, true);
		pvmove = GetHashLine( depth, pos);
		bestmove = pos->pvarray[0]; 
		cout<<"At Depth : "<<depth<<"With BestScore : "<<bestscore<<" BestMove : "<<PrMove(bestmove)<<" Nodes Visited : "<<info->nodes<<endl;
		
		pvmove = GetHashLine( depth, pos);
		out<<" Principal Variation (Hash-Table) ";
		for(int pvnum=0;pvnum<pvmove;pvnum++)
		{
			cout<<" "<<PrMove(bestmove);
		}cout<<endl;
	}
}

