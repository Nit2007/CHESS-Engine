#include "defs.h"
#include "struct.h"

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
int SQ120(int sq64) {
    return sq64tosq120[sq64];
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
