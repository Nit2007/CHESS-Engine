#include "defs.h"
#include "struct.h"


int FilesBrd[BOARD_SQ_NUM];
int RanksBrd[BOARD_SQ_NUM];

int square120[8][8];
uint64_t PieceKeys[13][120];
uint64_t SideKey;
uint64_t CastleKeys[16]; 

void InitFilesRanksBrd()
{
    int i,sq,file,rank; 
    for( i=0;i<BOARD_SQ_NUM;i++)
    {
        FilesBrd[i]=OFFBOARD;
        RanksBrd[i]=OFFBOARD;
    }
    for( rank=RANK_1;rank<=RANK_8;rank++)
    {
        for( file=FILE_A;file<=FILE_H;file++)
        {
            sq=smalltobig(file,rank);
            FilesBrd[sq]=file;
            RanksBrd[sq]=rank;
        }
    }
    cout<<"FILES ARRAY "<<endl;
    for( i=0;i<BOARD_SQ_NUM;i++)
    {
        if(i%10==0)cout<<endl;
        cout<<FilesBrd[i]<<" ";
    }
    cout<<"RANK ARRAY "<<endl;
    for( i=0;i<BOARD_SQ_NUM;i++)
    {
        if(i%10==0)cout<<endl;
        cout<<RanksBrd[i]<<" ";
    }
}


void initsquare120(){
    for(int rank=0;rank<8;rank++)
    {
        for(int file=0;file<8;file++)
        {
            square120[rank][file]=smalltobig(file,rank);
        }
    }
}

void InitHashKeys() {
    for (int index = 0; index < 13; ++index) {
        for (int index2 = 0; index2 < 120; ++index2) {
            PieceKeys[index][index2] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for (int index = 0; index < 16; ++index) {
        CastleKeys[index] = RAND_64;
    }
}

void allinit()
{
    init120to64();
    initsquare120();
    InitHashKeys();
    InitFilesRanksBrd();
}
