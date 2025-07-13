#include "defs.h"
#include "struct.h"

int sq64tosq120[64];        //8 * 8
int sq120tosq64[BOARD_SQ_NUM];// 10 * 12

int smalltobig(int file,int rank)
{
    return (file+21)+(rank*10);
}

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
