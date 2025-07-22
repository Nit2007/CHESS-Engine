#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
    
    s_board B;
    Parse_FEN(START_FEN,B);
    PrintBoard(B);
    Parse_FEN(FEN_1,B);
    PrintBoard(B);
    Parse_FEN(FEN_2,B);
    PrintBoard(B);
    Parse_FEN(FEN_3,B);
    PrintBoard(B);
    
    cout<<"*************end*************"<<endl<<endl<<endl;
}
