#include "Defs.hpp"
#include "Struct.hpp"

extern int sq64tosq120[64];
extern int sq120tosq64[BOARD_SQ_NUM];
extern int square120[8][8];


int main()
{
    allinit();
    uint64_t bb=0ULL;
    PrintBitBoard(bb);   
    //bb=8ULL;
    
    int sq120=getSquareFromString("d2");
    bb|=(1ULL<<SQ64(sq120));
    cout<<"COUNT : "<<countBitBoard(bb)<<endl;
    PrintBitBoard(bb);  

    sq120=getSquareFromString("d3");
    bb|=(1ULL<<SQ64(sq120));
    cout<<"COUNT : "<<countBitBoard(bb)<<endl;
    PrintBitBoard(bb);

    sq120=getSquareFromString("d4");
    bb|=(1ULL<<SQ64(sq120));
    cout<<"COUNT : "<<countBitBoard(bb)<<endl;
    PrintBitBoard(bb);

    sq120=getSquareFromString("d7");
    bb|=(1ULL<<SQ64(sq120));
    cout<<"COUNT : "<<countBitBoard(bb)<<endl;
    PrintBitBoard(bb);

    cout<<"POPPED : "<<popBitBoard(&bb)<<endl; PrintBitBoard(bb);
    cout<<"POPPED : "<<popBitBoard(&bb)<<endl; PrintBitBoard(bb);
    cout<<"POPPED : "<<popBitBoard(&bb)<<endl; PrintBitBoard(bb);

    uint64_t playbitboard = 0ULL;
    for(int i=0;i<64;i++)
        {
        cout<<"index : "<<i<<endl;
        PrintBitBoard(setBitBoard[i]);
        cout<<endl;
        }
    cout<<"-------------------------------------------------------------------------------";
    PrintBitBoard(bb);
    setBit(bb,62);PrintBitBoard(bb);
    clearBit(bb,62);PrintBitBoard(bb);
    
   
    //ASSERT(5==4);
    cout<<"end";
}
