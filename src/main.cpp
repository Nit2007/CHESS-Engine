#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
    s_board board;
    Parse_FEN((char*)FEN_2,&board);
    PrintBoard(&board);
    s_movelist list;
    GenerateAllMoves(&board,&list);
    PrintMoveList(&list);
    s_searchinfo* info = new s_searchinfo;
    info->depth = 6; // Set search depth
    ClearForSearch(&board,info);
    SearchPosition(&board,info);
    cout<<endl<<"*************end*************"<<endl<<endl<<endl;
    
   // delete info;
    return 0;
}
