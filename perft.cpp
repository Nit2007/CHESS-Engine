#include "defs.h"
#include "struct.h"

long long int leafNode;

void Perft(int depth , s_board*pos)
{
   if(depth == 0)
   {
       leafNode++;
       return ;
   }
   s_movelist list;
    GenerateAllMoves(pos,&list);
  for(int movenum=0;movenum<list.count;movenum++)
  {
     if(!MakeMove(pos,list.moves[movenum].move))continue;
     Perft(depth - 1,pos);
     TakeMove(pos);
  }
  return;
}
