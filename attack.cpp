#include "defs.h"
#include "struct.h"

const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };

int SqAttacked(const int sq, const int side, const s_board *pos) {// sq - 120 based index
  if(side==WHITE)
  {
      if(pos->pieces[sq-11]==WP || pos->pieces[sq-9]==WP)return true;
  }
  else
  {
       if(pos->pieces[sq+11]==BP || pos->pieces[sq+9]==BP)return true;
  }

  return 0;
}
