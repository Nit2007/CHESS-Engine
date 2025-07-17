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
    int enpas;
    int fifty;
    uint64_t poskey;
};

struct s_board
{
    int pieces[BOARD_SQ_NUM];
    
    uint64_t pawns[3];
    int king[3];
    int bigpce[3];
    int majpce[3];
    int minpce[3];

    int side;
    int enpas;
    int fifty;
    
    int ply;
    int hisply;
    //int castleperm;
    
    uint64_t poskey;

    int piecelist[13][10];
    s_undo history[MAXGAMEMOVES];//vector<s_undo>history(MAXGAMEMOVES);


};

enum Piece {EMPTY, WP, WN, WB, WR, WQ, WK,BP, BN, BB, BR, BQ, BK};// Piece types
enum File {FILE_A, FILE_B, FILE_C, FILE_D,FILE_E, FILE_F, FILE_G, FILE_H,FILE_NONE};// File (column) indices
enum Rank {RANK_1, RANK_2, RANK_3, RANK_4,RANK_5, RANK_6, RANK_7, RANK_8,RANK_NONE};// Rank (row) indices
enum Side {WHITE, BLACK, BOTH};// Side to move
enum Square {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8,
    NO_SQ};// Board squares (in 120-square representation)
enum Bool {FALSE, TRUE};


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
uint64_t setBitMask[64];
uint64_t clearBitMask[64];
void initBitMask()
{
    for(int index=0;index<64;index++)
        {
            setBitMask[index]=0ULL;
            clearBitMask[index]=0ULL;
        }
    for(int index=0;index<64;index++)
        {
            setBitMask[index]= (1ULL<<index);
            clearBitMask[index]= ~setBitMask[index];
        }
}

void setBit(uint64_t &bb , int sq)
{
    bb|=setBitMask[sq];
}


void clearBit(uint64_t &bb , int sq)
{
     bb&=clearBitMask[sq];
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
    uint64_t bp=0ULL;
    initBitMask();
   setBit(bp,60);PrintBitBoard(bp);
   clearBit(bp,60); PrintBitBoard(bp);
    //ASSERT(5==4);
    cout<<"end"<<endl;
}
