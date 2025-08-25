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

