#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
     
    s_board B;
    Parse_FEN((char*)START_FEN, &B); // cast and address-of for pointer
    PrintBoard(&B);
    Parse_FEN((char*)FEN_1, &B);
    PrintBoard(&B);
    Parse_FEN((char*)FEN_2, &B);
    PrintBoard(&B);
    Parse_FEN((char*)FEN_3, &B);
    PrintBoard(&B);
    
    
    cout<<endl<<"*************end*************"<<endl<<endl<<endl;
}
