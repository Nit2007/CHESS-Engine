#include "defs.h"
#include "struct.h"

long long int leafNode;

void Perft(int depth , s_board*pos)
{//ecursively counts all leaf nodes (positions) reachable to a given depth.
   if(depth == 0)
   {// Base case: reached leaf node (depth exhausted)
       leafNode++;
       return ;
   }
   s_movelist list;
    GenerateAllMoves(pos,&list);
   int move=0;
  for(int movenum=0;movenum<list.count;movenum++)
  {move=list.moves[movenum].move;
     if(!MakeMove(pos,move))continue;
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
   int start = GetTimeMs();
   s_movelist list;
    GenerateAllMoves(pos,&list);
   int move=0;
  for(int movenum=0;movenum<list.count;movenum++)
  {
     move=list.moves[movenum].move;
     if(!MakeMove(pos,move))continue;
     // Snapshot total nodes visited so far (before exploring this move)
     long long int culmulativeNode=leafNode;   //cumulativeNode = leaf count before move
     Perft(depth - 1,pos);
     TakeMove(pos);
     // Difference = number of new nodes discovered from this move only
     long long int oldNode=leafNode-culmulativeNode;   //oldNode = (new total) − (old total) = nodes from this move
     cout<<"Move "<<movenum+1<<" : "<<PrMove(move)<<" : "<<oldNode<<endl;
  }
   cout<<"\nTest Completed , "<<leafNode<<" NODES visited in "<< GetTimeMs()-start<<" ms"<<endl;   // Print grand total after all moves explored


}
