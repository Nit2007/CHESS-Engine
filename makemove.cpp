#include <defs.h>
#include <struct.h>

#define HASH_PCE(pce,sq) (pos->poskey^=(PieceKeys[pce][sq]))
#define HASH_CA (pos->poskey^=(CastleKeys[pos->castleperm]))
#define HASH_SIDE (pos->poskey^=(SideKey))
#define HASH_EP (pos->poskey^=(PieceKeys[EMPTY][pos->enpas]))

const int CastlePerm[120] = {
    15,15,15,15,15,15,15,15,15,15,
    15,11,15,15,15, 3,15,15,14,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,
    15,13,15,15,15,12,15,15, 7,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15
};

void ClearPiece(const int sq,s_board* pos)
{
   ASSERT(SqOnBoard(sq));
   int pce=pos->pieces[sq];
   ASSERT(PieceValid(pce));
  int col=PieceCol[pce];
  int index=0;
  int t_piecenum=-1;
  HASH_PCE(pce,sq);

  pos->pieces[sq]=EMPTY;
  pos->material[col]-=PieceVal[pce];

  if(pieceBig[pce])
  {
    pos->bigpce[col]--;
    if(pieceMaj[pce])
    {
      pos->majpce[col]--;
    }
    else  pos->minpce[col]--;
  }
  else
  {
     clearBit(pos->pawns[col] , SQ64(sq));
     clearBit(pos->pawns[BOTH] , SQ64(sq));
  }// We want to remove a piece 'pce' from square 'sq'
// Every piece type has a list of the squares it occupies (piecelist[pce])
// and a count of how many there are (piecenum[pce])
for(int index = 0; index < pos->piecenum[pce]; index++) {
    if(pos->piecelist[pce][index] == sq) {// Look through the list of squares occupied by this piece type
        t_piecenum = index;// Found the entry for the square we want to clear
        break;
    }
}
ASSERT(t_piecenum != -1);// Make sure we actually found it
pos->piecenum[pce]--;// Decrease the count of this piece type
// Replace the removed square with the last element in the list
pos->piecelist[pce][t_piecenum] = pos->piecelist[pce][pos->piecenum[pce]];//Nexttime we only loop till (pos->piecenum-1) as we decremented it
}

void AddPiece(const int sq,s_board* pos,const int pce)
{
  ASSERT(SqOnBoard(sq));
   ASSERT(PieceValid(pce));
  int col=PieceCol[pce];
  
  HASH_PCE(pce,sq);

  pos->pieces[sq]=pce;
  pos->material[col]+=PieceVal[pce];
  if(pieceBig[pce])
  {
    pos->bigpce[col]++;
    if(pieceMaj[pce])
    {
      pos->majpce[col]++;
    }
    else  pos->minpce[col]++;
  }
  else
  {
     setBit(pos->pawns[col] , SQ64(sq));
     setBit(pos->pawns[BOTH] , SQ64(sq));
  }
  pos->piecelist[pce][pos->piecenum[pce]++]=sq;
}

void MovePiece(const int from,const int to,s_board* pos)
{
    ASSERT(SqOnBoard(from));
  ASSERT(SqOnBoard(to));
  int pce=pos->pieces[from];
  int col=PieceCol[pce];
#ifdef DEBUG
  int t_piecenum=FALSE;
#endif
  HASH_PCE(pce,from);
pos->pieces[from]=EMPTY;
  HASH_PCE(pce,to);
pos->pieces[to]=pce;
  if(!pieceBig[pce])
  {
    clearBit(pos->pawns[col] , SQ64(from));
     clearBit(pos->pawns[BOTH] , SQ64(from));
    setBit(pos->pawns[col] , SQ64(to));
     setBit(pos->pawns[BOTH] , SQ64(to));
  }
  for(int index = 0; index < pos->piecenum[pce]; index++) {
    if(pos->piecelist[pce][index] == from) 
    {
       pos->piecelist[pce][index] = to;
#ifdef DEBUG
        t_piecenum=TRUE;
#endif
        break;
    }
  }
  ASSERT(t_piecenum);
}

int MakeMove(s_board*pos ,int move)
{
    ASSERT(CheckBoard(pos));
    int from = FROMSQ(move);
    int to   = TOSQ(move);
  ASSERT(SqOnBoard(from));
  ASSERT(SqOnBoard(to));
  ASSERT(SideValid(side));
  ASSERT(PieceValid(pos->pieces[from]));
    int side=pos->side;
    pos->history[pos->hisply].poskey=pos->poskey;
    if(move & MFLAGEP )
    {
        if(side==WHITE)
        {
            ClearPiece( (to-10),pos);
        }
        else
        {
            ClearPiece((to+10),pos);
        }
    }
    else if(move & MFLAGCA  )
    {
        switch(to) {
            case C1:
                MovePiece(A1, D1, pos);
			break;
            case C8:
                MovePiece(A8, D8, pos);
			break;
            case G1:
                MovePiece(H1, F1, pos);
			break;
            case G8:
                MovePiece(H8, F8, pos);
			break;
            default: ASSERT(FALSE); break;
        }
    }HASH_CA;
    if(pos->enPas != NO_SQ) HASH_EP;
    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPas = NO_SQ;

	HASH_CA;

    int cap=CAPTURED(move);
    pos->fifty++;
    if(cap!=EMPTY)
    {
        ASSERT(PieceValid(cap));
        pos->fifty=0;
        ClearPiece(to,pos);
    }
    pos->ply++;
    pos->hisply++;
    if(PiecePawn[pos->pieces[from] ])
    {pos->fifty=0;
        if(move & MFLAGPS) {
            if(side==WHITE) {
                pos->enPas=from+10;
                ASSERT(RanksBrd[pos->enPas]==RANK_3);
            } else {
                pos->enPas=from-10;
                ASSERT(RanksBrd[pos->enPas]==RANK_6);
            }
            HASH_EP;
    }
     MovePiece(from,to,pos);

     int prom=PROMOTED(move);
     if(move & MFLAGPROM)
     {
         ASSERT(PieceValid(prom) && !PiecePawn[prom] && !PieceKing[prom]);
         ClearPiece(to,pos);
         AddPiece(to,pos,prom);
     }
     if(PieceKing[pos->pieces[to]]) {
        pos->king[pos->side] = to;
    }
	pos->side ^= 1;
    HASH_SIDE;
ASSERT(CheckBoard(pos));
	if(SqAttacked(pos->king[side],pos->side,pos))  {
        TakeMove(pos);
        return FALSE;
    }
	return TRUE;
}


