#include "defs.h"
#include "struct.h"

#define MOVE(from,to,captured,promoted,flag) ( (from) | (to<<7) | (captured<<14) | (promoted<<20) | (flag) )
#define SQOFFBOARD(sq) (FilesBrd[sq]==OFFBOARD)

const int LoopSlidePce[8] = {WB,WR,WQ,0,BB,BR,BQ,0};
const int LoopNonSlidePce[8] = {WN,WK,0,BN,BK,0};
const int LoopSlideIndex[2] = {0,4};
const int LoopNonSlideIndex[2] = {0,3};

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

int MoveExists(s_board*pos,const int move)
{
	s_movelist list;
    GenerateAllMoves(pos, &list);
	for(int movenum=0;movenum<list.count;movenum++)
	{
		if(!MakeMove(pos,move))continue;
		TakeMove(pos);
		if(list.moves[movenum].move == move) return TRUE;
	}
	return FALSE;
}


void AddQuietMove(const s_board *pos, int move, s_movelist *list) {
    ASSERT(list->count < MAX_POSITION_MOVES);
    list->moves[list->count].move = move;
    list->moves[list->count].score = ScoreMove(move, pos);
    list->count++;
}

// Replace your existing AddCaptureMove function with this:
void AddCaptureMove(const s_board *pos, int move, s_movelist *list) {
    ASSERT(list->count < MAX_POSITION_MOVES);
    list->moves[list->count].move = move;
    list->moves[list->count].score = ScoreMove(move, pos);
    list->count++;
}

// Replace your existing AddEnpasMove function with this:
void AddEnpasMove(const s_board *pos, int move, s_movelist *list) {
    ASSERT(list->count < MAX_POSITION_MOVES);
    list->moves[list->count].move = move;
    list->moves[list->count].score = ScoreMove(move, pos);
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
	list->count=0;
    int sq=0;
    int t_sq=0;
    int pce = EMPTY;
    int dir=0;
    int index=0;
    int pceIndex=0;
    //cout<<"Side to MOVE : "<<pos->side<<endl;
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
		if(pos->castleperm & WKCA)
		{
			if(pos->pieces[F1]==EMPTY && pos->pieces[G1]==EMPTY)
			{
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(F1,BLACK,pos) && !SqAttacked(G1,BLACK,pos))
				AddQuietMove(pos, MOVE(E1,G1,EMPTY,EMPTY,MFLAGCA), list);//cout<<"WKCA Possible\n";
			}
		}
		if(pos->castleperm & WQCA)
		{
			if(pos->pieces[D1]==EMPTY && pos->pieces[C1]==EMPTY && pos->pieces[B1]==EMPTY)
			{
				if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(D1,BLACK,pos) && !SqAttacked(C1,BLACK,pos))
				AddQuietMove(pos, MOVE(E1,C1,EMPTY,EMPTY,MFLAGCA), list);//cout<<"WQCA Possible\n";
			}
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
		if(pos->castleperm & BKCA)
		{
			if(pos->pieces[F8]==EMPTY && pos->pieces[G8]==EMPTY)
			{
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(F8,WHITE,pos) && !SqAttacked(G8,WHITE,pos))
				AddQuietMove(pos, MOVE(E8,G8,EMPTY,EMPTY,MFLAGCA), list);//cout<<"BKCA Possible\n";
			}
		}
		if(pos->castleperm & BQCA)
		{
			if(pos->pieces[D8]==EMPTY && pos->pieces[C8]==EMPTY && pos->pieces[B8]==EMPTY)
			{
				if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(D8,WHITE,pos) && !SqAttacked(C8,WHITE,pos))
				AddQuietMove(pos, MOVE(E8,C8,EMPTY,EMPTY,MFLAGCA), list);//cout<<"BQCA Possible\n";
			}
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
		//cout<<"SLIDERS PIECE: "<<pceChar[pce]<<"  PIECEINDEX: "<<pceIndex<<endl;
		for(int pcenum=0;pcenum<pos->piecenum[pce];pcenum++)
		{
			sq = pos->piecelist[pce][pcenum];
			ASSERT(SqOnBoard(sq));
			//cout<<"Piece "<<pceChar[pce]<<" on "<<PrSq(sq)<<endl;
			for(index=0;index<NumDir[pce];index++)
			{
				dir = PceDir[pce][index];
				t_sq = sq + dir;
				while(!SQOFFBOARD(t_sq))
				{

					if(pos->pieces[t_sq] !=EMPTY)
					{
						if( pieceCol[ pos->pieces[t_sq] ] == (pos->side^1))
						{
							AddCaptureMove(pos, MOVE(sq,t_sq,pos->pieces[t_sq],EMPTY,0), list);//cout<<"     Capture on "<<PrSq(t_sq)<<endl;
						}
						break;
					}
					AddQuietMove(pos, MOVE(sq,t_sq,EMPTY,EMPTY,0), list);//cout<<"     Attack on "<<PrSq(t_sq)<<endl;
					t_sq += dir;
				}
			}
		}
        pce=LoopSlidePce[pceIndex++];
	}
    // NON - SLIDERS  { N , K }
    pceIndex=LoopNonSlideIndex[pos->side];
    pce=LoopNonSlidePce[pceIndex++];
	while(pce!=0)
	{
		ASSERT(PieceValid(pce));
		//cout<<"Non-SLIDERS PIECE: "<<pceChar[pce]<<"  PIECEINDEX: "<<pceIndex<<endl;
		for(int pcenum=0;pcenum<pos->piecenum[pce];pcenum++)
		{
			sq = pos->piecelist[pce][pcenum];
			ASSERT(SqOnBoard(sq));
			//cout<<"Piece "<<pceChar[pce]<<" on "<<PrSq(sq)<<endl;
			for(index=0;index<NumDir[pce];index++)
			{
				dir = PceDir[pce][index];
				t_sq = sq + dir;
				if(SQOFFBOARD(t_sq))continue;

				if(pos->pieces[t_sq] !=EMPTY)
				{
					if( pieceCol[ pos->pieces[t_sq] ] == (pos->side^1))
					{
						AddCaptureMove(pos, MOVE(sq,t_sq,pos->pieces[t_sq],EMPTY,0), list);//cout<<"     Capture on "<<PrSq(t_sq)<<endl;
					}
					continue;
				}
				AddQuietMove(pos, MOVE(sq,t_sq,EMPTY,EMPTY,0), list);//cout<<"     Attack on "<<PrSq(t_sq)<<endl;
			}
		}
    pce=LoopNonSlidePce[pceIndex++];
	}
    
}
