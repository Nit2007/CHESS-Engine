#include "defs.h"
#include "struct.h"

void ClearHashTable(s_hashtable *table) {
    s_hashentry *tableEntry;
    
    // Loop through each entry in the hash table
    for ( tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; tableEntry++) {
        tableEntry->posKey = 0ULL;    // Clear position key
        tableEntry->move = FALSE;    // Set to no move
        tableEntry->depth = 0;        // Clear search depth
        tableEntry->score = 0;        // Clear evaluation score
        tableEntry->flags = 0;        // Clear entry flags
    }
    table->newWrite = 0;  // Reset new write counter
}

void InitHashTable(s_hashtable *table, const int MB) {
    // Calculate hash table size in bytes
    int HashSize = 0x100000 * MB;  // 0x100000 = 1MB in bytes {16^5 = 1,048,576 }
    table->numEntries = HashSize / sizeof(s_hashentry);
    table->numEntries -= 2;  // Safety margin
    
    // Free existing table if it exists
    if (table->pTable != NULL) {
        free(table->pTable); // Prevent memory leaks
    }
    
    // Allocate memory for new hash table
    table->pTable = (s_hashentry *) malloc(table->numEntries * sizeof(s_hashentry));
    
    if (table->pTable == NULL) {// Allocation failed, try with half the memory
        printf("Hash Allocation Failed, trying %dMB...\n", MB/2);
        InitHashTable(table, MB/2);
    } else {// Success - clear the table and report
        ClearHashTable(table);
        printf("HashTable init complete with %d entries\n", table->numEntries);
    }
}


