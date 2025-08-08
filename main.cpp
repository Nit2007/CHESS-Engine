#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
    int from = 26;
    int to = 28;
    int cap=WR;int prom=BQ;
    int move=0;
    move= ( from | (to << 7) | (cap <<14) | (prom << 20)) ; 
    cout<< move<<endl;
    cout<<endl<<"*************end*************"<<endl<<endl<<endl;
}
