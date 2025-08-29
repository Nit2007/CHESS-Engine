#include "defs.h"
#include "struct.h"

char *PrSq(const int sq)
{
    static char sqstr[3];
    int file=FilesBrd[sq];
    int rank=RanksBrd[sq];
    sqstr[0]='a'+file;
    sqstr[1]='1'+rank;//sqstr[2] = '\0';
  return sqstr;
}

char *PrMove(const int move)
{
    static char mvstr[6];
    int filefrom=FilesBrd[FROMSQ(move)];
    int rankfrom=RanksBrd[FROMSQ(move)];
    int fileto=FilesBrd[TOSQ(move)];
    int rankto=RanksBrd[TOSQ(move)];
    int promoted=PROMOTED(move);
    if(promoted)
    {
      char pchar='q';
      if(IsKn(promoted))pchar='n';
      if(IsRQ(promoted) && !IsBQ(promoted))pchar='r';
      if(!IsRQ(promoted) && IsBQ(promoted))pchar='b';
       mvstr[4]=pchar;
       mvstr[5]='\0';
    }
  else mvstr[4]='\0';
    mvstr[0]='a'+filefrom;
    mvstr[1]='1'+rankfrom;
    mvstr[2]='a'+fileto;
    mvstr[3]='1'+rankto;

  return mvstr;
}

void PrintMoveList(const s_movelist *list)
{
    int score=0;    int move=0;
    cout<<"MoveList:"<<endl;
    for(int index=0;index<list->count;index++)
    {
        move=list->moves[index].move;
        score=list->moves[index].score;
        cout<<"Move "<<index+1<<" > "<<PrMove(move)<<" ("<<score<<")"<<endl;
    }
    cout<<"The Total Number of Moves : "<<list->count<<endl;
}

int ParseMove(char* ptchar,s_board*pos)
{
    ASSERT(CheckBoard(pos));
    if(ptchar[0] <'a' && ptchar[0]>'h')return FALSE;
    if(ptchar[1] <'1' && ptchar[1]>'8')return FALSE;
    if(ptchar[2] <'a' && ptchar[2]>'h')return FALSE;
    if(ptchar[3] <'1' && ptchar[3]>'8')return FALSE;
    s_movelist *list;
    GenerateAllMoves(pos,list);
    int move=0;
  for(int movenum=0;movenum<list.count;movenum++)
  {
      move=list.moves[movenum].move;
      int from=FROMSQ(move);
      int to=TOSQ(move);
      ASSERT(SqOnBoard(from) && SqOnBoard(to));
  }
}
