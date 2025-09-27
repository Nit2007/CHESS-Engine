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
    if (strlen(ptchar) < 4) return FALSE;
    if(ptchar[0] <'a'  ||  ptchar[0]>'h')return FALSE;
    if(ptchar[1] <'1'  ||  ptchar[1]>'8')return FALSE;
    if(ptchar[2] <'a'  ||  ptchar[2]>'h')return FALSE;
    if(ptchar[3] <'1'  ||  ptchar[3]>'8')return FALSE;
    int From = getSquareFromString (string(1, ptchar[0]) + string(1, ptchar[1]) );
    int To = getSquareFromString(string(1, ptchar[2]) + string(1, ptchar[3]));
    ASSERT(SqOnBoard(From) && SqOnBoard(To));
    //cout<<"Ptchar : "<<ptchar<<" Fromsq : "<<From<<" Tosq : "<<To<<endl;
    s_movelist list;
    GenerateAllMoves(pos,&list);
    int move=0;
    int prompce=EMPTY;
  for(int movenum=0;movenum<list.count;movenum++)
  {
      move=list.moves[movenum].move;
      int from=FROMSQ(move);
      int to=TOSQ(move);
      ASSERT(SqOnBoard(from) && SqOnBoard(to));
      if(From == from && To== to)
      {
          prompce=PROMOTED(move);
          if (prompce != EMPTY) {
          if (strlen(ptchar) < 5) continue; // No promotion piece given
          if(IsRQ(prompce) && !IsBQ(prompce) && ptchar[4]=='r')return move;
          else if(!IsRQ(prompce) && IsBQ(prompce) && ptchar[4]=='b')return move;
          else if(IsRQ(prompce) && IsBQ(prompce) && ptchar[4]=='q')return move;
          else if(IsKn(prompce) && ptchar[4]=='n')return move;
          continue;// If promotion piece doesn't match, continue to next move
        }
          return move; // If not Promotion then we have found out the UnIQUE Move
      }
   }

   // Handle castling moves (e1g1, e1c1, e8g8, e8c8)
   if (strlen(ptchar) == 4) {
       int from = getSquareFromString(string(1, ptchar[0]) + string(1, ptchar[1]));
       int to = getSquareFromString(string(1, ptchar[2]) + string(1, ptchar[3]));

       if (pos->pieces[from] == WK && from == E1) {
           if (to == G1 && (pos->castleperm & WKCA)) return MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA);
           if (to == C1 && (pos->castleperm & WQCA)) return MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA);
       }
       else if (pos->pieces[from] == BK && from == E8) {
           if (to == G8 && (pos->castleperm & BKCA)) return MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA);
           if (to == C8 && (pos->castleperm & BQCA)) return MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA);
       }
   }

    return false;
}
