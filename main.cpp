#include <iostream>
using namespace std;
#include <cstdlib> //FOR EXIT()
#define MAXGAMEMOVES 2048
#define BOARD_SQ_NUM 120

#define DEBUG 
#ifndef DEBUG      //checks if debug is defined
#define ASSERT(n)
#else
    #define ASSERT(n)                       \
        if(!n){                             \
            cerr<<#n<<"-failed\n";           \
            cerr<<"on"<<__DATE__<<endl;         \
            cerr<<"at"<<__TIME__<<endl;         \
            cerr<<"in file "<<__FILE__<<endl;   \
            cerr<<"at line" << __LINE__<<endl;  \
            exit(1)                  ;}

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

int smalltobig(int f,int r)
{
    return (f+21)+(r*10);
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
        cout<<sq64tosq120[i]<<" ";
        if(i%8==0)cout<<endl;
    }
    for(int i=0;i<BOARD_SQ_NUM;i++)
    {
        cout<<sq120tosq64[i]<<" ";
        if(i%10==0)cout<<endl;
    }
}
int main()
{
    init120to64();
    print12064();
    cout<<endl;
    int x=5;
    ASSERT(x==4);
    cout<<"end";
}


