#include "defs.h"
#include "struct.h"

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };


int SqAttacked(const int sq, const int side, const s_board *pos) {// sq - 120 based index
  int pce,dir,t_sq;
  ASSERT(SqOnBoard(sq));
  ASSERT(SideValid(side));
  ASSERT(CheckBoard(pos));
  if(side==WHITE)//aattacking side 
  {
      if(pos->pieces[sq-11]==WP || pos->pieces[sq-9]==WP)return true;
  }
  else
  {
       if(pos->pieces[sq+11]==BP || pos->pieces[sq+9]==BP)return true;
  }
  for(int i=0;i<8;i++)
  {// Knight
      pce=pos->pieces[sq+KnDir[i]];
      if( (pce != OFFBOARD && pce != EMPTY) && IsKn(pce) && pieceCol[pce]==side)return true;
  }

    if (pos->piecenum[side == WHITE ? WR : BR] > 0 || pos->piecenum[side == WHITE ? WQ : BQ] > 0) {
        uint64_t attacksRook = GetRookAttacks(SQ64(sq), pos->occupied[BOTH]) & pos->occupied[side];
        while (attacksRook) {
            int sq64 = popBitBoard(&attacksRook);
            pce = pos->pieces[SQ120(sq64)];
            if (IsRQ(pce)) return true;
        }
    }

    if (pos->piecenum[side == WHITE ? WB : BB] > 0 || pos->piecenum[side == WHITE ? WQ : BQ] > 0) {
        uint64_t attacksBishop = GetBishopAttacks(SQ64(sq), pos->occupied[BOTH]) & pos->occupied[side];
        while (attacksBishop) {
            int sq64 = popBitBoard(&attacksBishop);
            pce = pos->pieces[SQ120(sq64)];
            if (IsBQ(pce)) return true;
        }
    }
    for(int i=0;i<8;i++)
    {
        pce=pos->pieces[sq + KiDir[i] ];
        if((pce != OFFBOARD && pce != EMPTY) && IsKi(pce) && pieceCol[pce]==side)return true;
    }
   
  
  return false;
}

void showSqAttackBySide(const int side, const s_board*pos)
{
    int rank=0,file=0,sq=0;
    cout<<"SQUARE ATTACKED BY : "<< ((side==0) ? "WHITE":"BLACK")<<endl;
    for(rank=RANK_8;rank>=RANK_1;rank--)
      {
         for(file=FILE_A;file<=FILE_H;file++)
           {
               sq=smalltobig(file,rank);
               if(SqAttacked(sq,side,pos))cout<<"X ";
             else cout<<"- ";
           }
        cout<<endl;
      }
}

