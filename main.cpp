#include <iostream>
using namespace std;
#include <cstdlib> //FOR EXIT()
#include <cstdint> //FOR uint64_t
#define MAXGAMEMOVES 2048
#define BOARD_SQ_NUM 120
// You define this only in debugging
#define DEBUG       

#ifndef DEBUG       //CHECKS IF DEBUG IS DEFINED
// In release mode: do nothing
    #define ASSERT(n)            
#else
// In debug mode: do actual checking        // Print error info and exit
    #define ASSERT(n)                           \
        if (!(n)) {                             \ 
            cerr << #n << " - Failed\n";        \
            cerr << "On " << __DATE__ << endl;  \
            cerr << "At " << __TIME__ << endl;  \
            cerr << "In File " << __FILE__ << endl; \
            cerr << "At Line " << __LINE__ << endl; \
            exit(1);                            \
        }
#endif
struct s_undo
{
    int move;
    int castleperm;
    //uint64_t poskey;
};

struct s_board
{
    int side;
    int pieces[BOARD_SQ_NUM];
    //uint64_t pawns[3];
    int king[3];
    //int bigpce[3];
    int majpce[3];
    int minpce[3];


    int castleperm;
    int fifty;
    int enpas;
    //uint64_t poskey;
    int piecelist[13][10];
    s_undo history[MAXGAMEMOVES];//vector<s_undo>history(MAXGAMEMOVES);


};

int smalltobig(int file,int rank)
{
    return (file+21)+(rank*10);
}
int sq64tosq120[64];        //8 * 8
int sq120tosq64[BOARD_SQ_NUM];// 10 * 12
void init120to64()
{
    int sq64=0;
    for(int i=0;i<BOARD_SQ_NUM;i++)
    {
        sq120tosq64[i]=65;
    }
    for(int i=0;i<64;i++)
    {
        sq64tosq120[i]=120;
    }

    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            int sq=smalltobig(j,i);
            sq64tosq120[sq64]=sq;
            sq120tosq64[sq]=sq64;
            sq64++;
        }
    }
}
void print12064()
{
    for(int i=0;i<64;i++)
    {
        if(i!=0 && i%8==0)cout<<endl;
        cout<<sq64tosq120[i]<<" ";
    }
    cout<<endl<<endl;
    for(int i=0;i<BOARD_SQ_NUM;i++)
    {
        if(i!=0 && i%10==0)cout<<endl;
        cout<<sq120tosq64[i]<<" ";
    }
}
int SQ64(int sq120) {
    return sq120tosq64[sq120];
}
void PrintBitBoard(uint64_t bb)    //PARAMETER(BITBOARD)
{
    uint64_t shiftme=1ULL;
    int sq=0;
    int sq64=0;
    for(int rank=7;rank>=0;rank--)
    {
        for(int file=0;file<8;file++)
            {
                sq=smalltobig(file,rank);   //passing file,rank to get 10 * 12 index
                sq64= SQ64(sq);
                //sq64++; 
                if((bb & (shiftme<<sq64))!=0)   //IF PIECE PRESENT
                cout<<"X ";
                else cout<<"- ";
            }
        cout<<endl;
    }
    cout<<endl<<endl;
}
//FOR CONVERTING CHESS SQUARE NAME TO (FILE,RANK)
int square120[8][8];
void initsquare120(){
for(int rank=0;rank<8;rank++)
    {
        for(int file=0;file<8;file++)
            {
                square120[rank][file]=smalltobig(file,rank);
            }
    }
}
int fileCharToIndex(char c) {
    return tolower(c) - 'a'; // 'a' = 0
}

int rankCharToIndex(char c) {
    return c - '1'; // '1' = 0
}

int getSquareFromString(const string& sq) {
    if (sq.length() != 2) return -1;
    int file = fileCharToIndex(sq[0]);
    int rank = rankCharToIndex(sq[1]);
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return square120[rank][file];
}

int popBitBoard(uint64_t *bb)//Removes (pops) the least significant bit (LSB) from the bitboard
{//Returns the index (0–63) of the bit that was removed
    if(*bb==0)return -1;
    uint64_t b=*bb;
    int index=0;
    
    while( (b & 1ULL) == 0 )
        {
            index++;
            b = b>>1;
        }
    *bb &= (*bb-1);
    return index;
}

int countBitBoard(uint64_t bb)
{
    int count=0;
    while( bb)
        {
            if( (bb & 1ULL))count++;
            bb = bb>>1;
        }
    return count;
}

int main()
{
    init120to64();
    //print12064();
    initsquare120();
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
    
   
    //ASSERT(5==4);
    cout<<"end";
}
