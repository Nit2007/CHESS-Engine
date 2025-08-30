#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
    s_board board;
    Parse_FEN((char*)PAWNMOVES_FEN,&board);
    PrintBoard(&board);
    s_movelist list;
    GenerateAllMoves(&board,&list);
    PrintMoveList(&list);
    cout<<endl<<"*************end*************"<<endl<<endl<<endl;
}

