#include "defs.h"
#include "struct.h"

void AddQuietMove(const s_board *pos,int move , const s_movelist *list)
{
    list->moves[list->count].move=move;
    list->moves[list->count].score=0;
    list->count++;
}
void AddCaptureMove(const s_board *pos,int move , const s_movelist *list)
{
    list->moves[list->count].move=move;
    list->moves[list->count].score=0;
    list->count++;
}

void AddEnpasMove(const s_board *pos,int move , const s_movelist *list)
{
    list->moves[list->count].move=move;
    list->moves[list->count].score=0;
    list->count++;
}

void GenerateAllMoves(const s_board *pos)
{
}
