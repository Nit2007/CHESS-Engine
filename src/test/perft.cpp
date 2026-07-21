#include "defs.h"
#include "struct.h"
#include <fstream>
#include <string>
#include <iomanip>

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

void RunPerftSuite(const string& filename, int depth) {
    (void)filename; // unused now; kept so perft_main.cpp doesn't need changes

    printf("RunPerftSuite (hardcoded) started. Depth: %d\n", depth);
    fflush(stdout);

    static const char* fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"
    };
    static const int fenCount_total = sizeof(fens) / sizeof(fens[0]);

    long long totalNodes = 0;
    int totalTimeMs = 0;

    cout << "\n========================================================" << endl;
    cout << "Running Perft Suite at Depth " << depth << " (hardcoded FENs)" << endl;
    cout << "========================================================" << endl;

    static s_board boardObj;
    s_board* board = &boardObj;
    memset(board, 0, sizeof(s_board));
    board->hashtable[0].pTable = NULL;

    for (int fenCount = 0; fenCount < fenCount_total; fenCount++) {
        string fen = fens[fenCount];
        cout << "Parsing FEN: " << fen << endl;
        Parse_FEN((char*)fen.c_str(), board);

        leafNode = 0;
        int start = GetTimeMs();

        s_movelist list;
        GenerateAllMoves(board, &list);
        for (int i = 0; i < list.count; i++) {
            if (!MakeMove(board, list.moves[i].move)) continue;
            Perft(depth - 1, board);
            TakeMove(board);
        }

        int duration = GetTimeMs() - start;
        if (duration == 0) duration = 1;

        double nps = (double)leafNode / (duration / 1000.0);

        cout << "FEN " << (fenCount+1) << ": " << fen << endl;
        cout << "  Nodes: " << leafNode << " | Time: " << duration << "ms | NPS: " << fixed << setprecision(0) << nps << endl;

        totalNodes += leafNode;
        totalTimeMs += duration;
    }

    cout << "========================================================" << endl;
    cout << "SUITE COMPLETED" << endl;
    cout << "Total FENs Tested: " << fenCount_total << endl;
    cout << "Total Nodes: " << totalNodes << endl;
    cout << "Total Time: " << totalTimeMs << " ms" << endl;

    if (totalTimeMs > 0) {
        double overallNpsM = (double)totalNodes / (totalTimeMs / 1000.0) / 1000000.0;
        cout << "OVERALL PERFORMANCE: " << fixed << setprecision(2) << overallNpsM << " M NPS" << endl;
    }
    cout << "========================================================" << endl;

}
