#include "defs.h"
#include "struct.h"

int GetHashLine(const int depth, s_board *pos) {
    ASSERT(depth < 64 && depth >= 1);

	int move = ProbeHashMove(pos);
	int count = 0;
	
	while(move != FALSE && count < depth) {
	    ASSERT(count < 64);
	    if( MoveExists(pos, move) ) {
			MakeMove(pos, move);
			pos->pvarray[count++] = move;
		} else break;
		move = ProbeHashMove(pos);	
	}
	int line_count = count;
	while(count-- > 0) {
        TakeMove(pos);
    }

	return line_count;
}

void ClearHashTable(s_hashtable *table) {
    s_hashentry *tableEntry;
    
    // Loop through each entry in the hash table
    for ( tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; tableEntry++) {
        tableEntry->poskey = 0ULL;    // Clear position key
        tableEntry->move = FALSE;    // Set to no move
        tableEntry->depth = 0;        // Clear search depth
        tableEntry->score = 0;        // Clear evaluation score
        tableEntry->flags = 0;        // Clear entry flags
    }
    table->newWrite = 0;  // Reset new write counter
}

void InitHashTable(s_hashtable *table, const int MB) {
    if (MB <= 1) return;
    // Calculate hash table size in bytes
    int HashSize = 0x100000 * MB ;  // 0x100000 = 1MB in bytes {16^5 = 1,048,576 }
    table->numEntries = HashSize / sizeof(s_hashentry);
    table->numEntries -= 2;  // Safety margin
    
    // Free existing table if it exists
    if (table->pTable != NULL) {
        free(table->pTable); // Prevent memory leaks
    }
    
    // Allocate memory for new hash table
    table->pTable = (s_hashentry *) malloc(table->numEntries * sizeof(s_hashentry));
    
    if (table->pTable == NULL) { // Allocation failed, try with half the memory
        #ifdef DEBUG
        fprintf(stderr, "Hash Allocation Failed, trying %dMB...\n", MB/2);
        #endif
        InitHashTable(table, MB/2);
    } else { // Success - clear the table and report
        ClearHashTable(table);
        #ifdef DEBUG
        fprintf(stderr, "HashTable init complete with %d entries\n", table->numEntries);
        #endif
    }
}
void StoreHashMove(s_board* pos,  int move,int score,int depth,int flags)
{
    int index = (pos->poskey) % (pos->hashtable->numEntries);
    ASSERT(index>=0 && index<=(pos->hashtable->numEntries-1));

     if (pos->hashtable->pTable[index].poskey == 0) {
        pos->hashtable->newWrite++;
    } else {
        pos->hashtable->overWrite++;
    }

    pos->hashtable->pTable[index].poskey=pos->poskey;
    pos->hashtable->pTable[index].move=move;
    pos->hashtable->pTable[index].score=score;
    pos->hashtable->pTable[index].depth=depth;
    pos->hashtable->pTable[index].flags=flags;
}

int ProbeHashMove(const s_board* pos)
{//Probing the TT for Move Ordering 
    int index = (pos->poskey) % (pos->hashtable->numEntries);
    ASSERT(index >= 0 && index <= (pos->hashtable->numEntries - 1));

    s_hashentry* entry = &pos->hashtable->pTable[index];
    if(entry->poskey == pos->poskey) {
        return entry->move;
    }

    return FALSE;  // No Move Found
}

int ProbeHashMove(s_board* pos,int* move,int*score,int*depth,int*alpha,int*beta)
{//Probing the TT for using the cached Evaluation 
    int index = (pos->poskey) % (pos->hashtable->numEntries);
    ASSERT(index>=0 && index<=(pos->hashtable->numEntries-1));

    s_hashentry* entry = &pos->hashtable->pTable[index];
    if(entry->poskey==pos->poskey && entry->depth >= *depth){
        *move  = entry->move;
        *score = entry->score;
        pos->hashtable[0].hit++;

        if(entry->flags == HFEXACT){
            return entry->move;
        }
        if(entry->flags == HFALPHA &&  entry->score <= *alpha ){
            *score = *alpha;
            return entry->move;
        }
        if (entry->flags == HFBETA && entry->score >= *beta) {
            *score = *beta;
            pos->hashtable[0].cut++;
            return entry->move;
        }
    }

     return FALSE;//No (Useable move/Move) Found
}


