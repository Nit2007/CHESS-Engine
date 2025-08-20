#include "defs.h"
#include "struct.h"

#define MOVE(from,to,captured,promoted,flag) ( (from) | (to<<7) | (captured<<14) | (promoted<<20) | (flag) )
#define SQOFFBOARD(sq) (FilesBrd[sq]==OFFBOARD)

int LoopSlidePce[8] = {WB,WR,WQ,0,BB,BR,BQ,0};
int LoopNonSlidePce[8] = {WN,WK,0,BN,BK,0};
int LoopSlideIndex[2] = {0,4};
int LoopNonSlideIndex[2] = {0,3};

const int PceDir[13][8] = {     //[piecetype][possiblemoves]
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = { 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

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
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(PieceValidEmpty(cap));
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
    
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    if(RanksBrd[from]==RANK_7)
    {
         AddQuietMove(pos, MOVE(from,to,EMPTY,WQ,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,WR,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,WB,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,WN,0) , list);
    }
    else  AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0) , list);
}

void AddBlackPawnCapMove(const s_board *pos , const int from , const int to ,const int cap ,   s_movelist *list)
{
    
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(PieceValidEmpty(cap));
    if(RanksBrd[from]==RANK_2)
    {
         AddCaptureMove(pos, MOVE(from,to,cap,BQ,0) , list);
         AddCaptureMove(pos, MOVE(from,to,cap,BR,0) , list);
         AddCaptureMove(pos, MOVE(from,to,cap,BB,0) , list);
         AddCaptureMove(pos, MOVE(from,to,cap,BN,0) , list);
    }
    else  AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0) , list);
}
void AddBlackPawnMove(const s_board *pos , const int from , const int to ,   s_movelist *list)
{
    
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    if(RanksBrd[from]==RANK_2)
    {
         AddQuietMove(pos, MOVE(from,to,EMPTY,BQ,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,BR,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,BB,0) , list);
         AddQuietMove(pos, MOVE(from,to,EMPTY,BN,0) , list);
    }
    else  AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0) , list);
}

void GenerateAllMoves(const s_board *pos ,  s_movelist *list)
{
    ASSERT(CheckBoard(pos));
    int sq=0;
    int t_sq=0;
    int pce = EMPTY;
    int dir=0;
    int index=0;
    int pceIndex=0;
    cout<<"Side to MOVE : "<<pos->side<<endl;
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
    else{
        for(int pcenum=0;pcenum<pos->piecenum[BP]; pcenum++)
        {
            sq=pos->piecelist[BP][pcenum];
            ASSERT(SqOnBoard(sq));

            if(pos->pieces[sq-10]==EMPTY)
            {
                AddBlackPawnMove(pos,sq,sq-10,list);
                if(RanksBrd[sq]==RANK_7 && pos->pieces[sq-20]==EMPTY)
                    AddQuietMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS) , list);
            }
            if(!SQOFFBOARD(sq-9) && pieceCol[pos->pieces[sq-9]]==WHITE)
                AddBlackPawnCapMove(pos,sq,sq-9,pos->pieces[sq-9],list);
            if(!SQOFFBOARD(sq-11) && pieceCol[pos->pieces[sq-11]]==WHITE)
                AddBlackPawnCapMove(pos,sq,sq-11,pos->pieces[sq-11],list);

            if(sq-9 == pos->enpas)
                AddCaptureMove(pos,MOVE(sq,(sq-9),EMPTY,EMPTY,MFLAGEP) ,list);
            if(sq-11 == pos->enpas)
                AddCaptureMove(pos,MOVE(sq,(sq-11),EMPTY,EMPTY,MFLAGEP) ,list);
        }
    }
                                        /*int LoopSlidePce[8] = {WB,WR,WQ,0,BB,BR,BQ,0};//defined above in movegen.cpp
                                          int LoopNonSlidePce[8] = {WN,WK,0,BN,BK,0};
                                          int LoopSlideIndex[2] = {0,4};
                                          int LoopNonSlideIndex[2] = {0,3};*/
	string pceChar=".PNBRQKpnbrqk";  
    //SLIDERS { B,R,Q }
    pceIndex=LoopSlideIndex[pos->side];
    pce=LoopSlidePce[pceIndex++];
	while(pce!=0)
	{
		ASSERT(PieceValid(pce));
		cout<<"SLIDERS PIECE:"<<pce<<"  PIECEINDEX: "<<pceIndex<<endl;
		
	}
    // NON - SLIDERS  { N , K }
    pceIndex=LoopNonSlideIndex[pos->side];
    pce=LoopNonSlidePce[pceIndex++];
	while(pce!=0)
	{
		ASSERT(PieceValid(pce));
		cout<<"Non-SLIDERS PIECE:"<<pce<<"  PIECEINDEX: "<<pceIndex<<endl;
		for(pcenum=0;pcenum<pos->piecenum[pce];pcenum++)
		{
			sq = pos->piecelist[pce][pcenum];
			ASSERT(SqOnBoard(sq));
			cout<<"Piece "<<pceChar[pce]<<"on "<<PrSq(sq)<<endl;
			for(index=0;index<NumDir[pce];index++)
			{
				dir = PceDir[pce][index];
				t_sq = sq + dir;
				if(SQOFFBOARD(t_sq))continue;

				if(pos->pieces[t_sq] !=EMPTY)
				{
					if( pieceCol[ pos->pieces[t_sq] ] == (pos->side^1))
					{
						cout<<"Capture on "<<PrSq(t_sq)<<endl;
					}
					continue;
				}
				cout<<"Attack on "<<PrSq(t_sq)<<endl;
			}
		}
	}
    
}
