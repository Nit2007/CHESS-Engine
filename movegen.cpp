#include "defs.h"
#include "struct.h"

#define MOVE(from,to,captured,promoted,flag) ( (from) | (to<<7) | (captured<<14) | (promoted<<20) | (flag) )
#define SQOFFBOARD(sq) (FilesBrd[sq]==OFFBOARD)

void AddQuietMove(const s_board *pos,int move ,  s_movelist *list)
{(void)pos;
    list->moves[list->count].move=move;
    list->moves[list->count].score=0;
    list->count++;
}
void AddCaptureMove(const s_board *pos,int move ,  s_movelist *list)
{(void)pos;
    list->moves[list->count].move=move;
    list->moves[list->count].score=0;
    list->count++;
}

void AddEnpasMove(const s_board *pos,int move ,  s_movelist *list)
{(void)pos;
    list->moves[list->count].move=move;
    list->moves[list->count].score=0;
    list->count++;
}

void AddWhitePawnCapMove(const s_board *pos , const int from , const int to ,const int cap ,   s_movelist *list)
{
    if(RanksBrd[from]==RANK_7)
    {
         AddCaptureMove(pos, MOVE(from,to,cap,WQ,0) , list);
         AddCaptureMove(pos, MOVE(from,to,cap,WR,0) , list);
         AddCaptureMove(pos, MOVE(from,to,cap,WB,0) , list);
         AddCaptureMove(pos, MOVE(from,to,cap,WN,0) , list);
    }
    else  AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0) , list);
}
void AddWhitePawnMove(const s_board *pos , const int from , const int to ,   s_movelist *list)
{
    if(RanksBrd[from]==RANK_7)
    {
         AddQuietMove(pos, MOVE(from,to,EMPTY,WQ,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,WR,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,WB,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,WN,0) , list);
    }
    else  AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0) , list);
}

void GenerateAllMoves(const s_board *pos ,  s_movelist *list)
{
    ASSERT(CheckBoard(pos));
    int sq=0;
    //int t_sq=0;
    //int pce = EMPTY;

    if(pos->side == WHITE)
    {
        for(int pcenum=0;pcenum<pos->piecenum[WP]; pcenum++)
        {
            sq=pos->piecelist[WP][pcenum];
            ASSERT(SqOnBoard(sq));

            if(pos->pieces[sq+10]==EMPTY)
            {
                AddWhitePawnMove(pos,sq,sq+10,list);
                if(RanksBrd[sq]==RANK_2 && pos->pieces[sq+20]==EMPTY)
                    AddQuietMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS) , list);
            }
            if(!SQOFFBOARD(sq+9) && pieceCol[pos->pieces[sq+9]]==BLACK)
                AddWhitePawnCapMove(pos,sq,sq+9,pos->pieces[sq+9],list);
            if(!SQOFFBOARD(sq+11) && pieceCol[pos->pieces[sq+11]]==BLACK)
                AddWhitePawnCapMove(pos,sq,sq+11,pos->pieces[sq+11],list);

            if(sq+9 == pos->enpas)
                AddCaptureMove(pos,MOVE(sq,(sq+9),EMPTY,EMPTY,MFLAGEP) ,list);
            if(sq+11 == pos->enpas)
                AddCaptureMove(pos,MOVE(sq,(sq+11),EMPTY,EMPTY,MFLAGEP) ,list);
        }
    }
    else{}
    
}
