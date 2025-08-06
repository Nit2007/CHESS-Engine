#include "defs.h"
#include "struct.h"

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };


int SqAttacked(const int sq, const int side, const s_board *pos) {// sq - 120 based index
  int pce,dir,t_sq;
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

  for(int i=0;i<4;i++)
  {//Queen ,Rook
      dir=RkDir[i];
      t_sq= sq + dir;
      pce=pos->pieces[t_sq];
      while(pce!=OFFBOARD)
      {
           if(pce!=EMPTY)
           {
               if(IsRQ(pce) && pieceCol[pce]==side)  return true;
               break;
           }
          t_sq+=dir;
          pce=pos->pieces[t_sq];
      }
    }
    for(int i=0;i<4;i++)
    {//Bishop
      dir=BiDir[i];
      t_sq= sq + dir;
      pce=pos->pieces[t_sq];
      while(pce!=OFFBOARD)
      {
           if(pce!=EMPTY)
           {
               if(IsBQ(pce) && pieceCol[pce]==side)  return true;
               break;
           }
          t_sq+=dir;
          pce=pos->pieces[t_sq];
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

