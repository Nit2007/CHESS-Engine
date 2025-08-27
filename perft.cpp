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
  {//int move=list.moves[movenum].move;
     if(!MakeMove(pos,list.moves[movenum].move))continue;
     Perft(depth - 1,pos);
     TakeMove(pos);
  }
  return;
}

void PerftTest(int depth,s_board*pos)
{
   ASSERT(CheckBoard(pos));
   cout<<"Starting to Depth : "<<depth<<endl;
   PrintBoard(pos);
   leafNode=0;
   s_movelist list;
    GenerateAllMoves(pos,&list);
  for(int movenum=0;movenum<list.count;movenum++)
  {
     if(!MakeMove(pos,list.moves[movenum].move))continue;
     long long int culmulativeNode=leafNode;
     Perft(depth - 1,pos);
     TakeMove(pos);
     long long int oldNode=leafNode-culmulativeNode;
     cout<<"Move "<<movenum+1<<" : "<<PrMove(move)<<" : "<<oldNode<<endl;
  }
   cout<<"\nTest Completed , "<<leafNode<<" NODES visited\n";
}
