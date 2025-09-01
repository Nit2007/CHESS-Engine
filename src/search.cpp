#include "defs.h"
#include "struct.h"

int IsRepetition(s_board* pos)
{
    for(int index=pos->hisply-pos->fifty;index<pos->hisply-1;index++)
      {    ASSERT(index>=0 && index <2048);
          if(pos->poskey == pos->history[index].poskey)return TRUE;
      }
  return FALSE;
}
