#include "Defs.hpp"
#include "Struct.hpp"

uint64_t setBitMask[64];
uint64_t clearBitMask[64];
// Bitboard utility functions

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
