#include "defs.h"
#include "struct.h"

int IsRepetition(s_board* pos)
{
    for(int index=pos->hisply-pos->fifty;index<pos->hisply-1;index++)
      {
          if(pos->poskey == pos->history[index].poskey)return TRUE;
      }
  return FALSE;
}
