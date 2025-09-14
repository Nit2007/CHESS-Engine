#pragma once
#include "defs.h"

// Represents a single move in the game.
struct s_move
{
    int move;    // An integer encoding the move details (from square, to square, captured piece, promotion piece, etc.).
    int score;   // A score assigned to the move, used for move ordering in the search algorithm to prioritize better moves.
};

// A list to store all generated moves for a given position.
struct s_movelist
{
    s_move moves[256]; // An array to hold all possible moves. 256 is a safe upper bound for moves from any position.
    int count;         // The number of moves currently stored in the list.
};

// Stores the necessary information to undo a move, allowing the engine to backtrack during search.
struct s_undo
{
    int move;         // The move that was made.
    int castleperm;   // The castling permissions before the move was made.
    int enpas;        // The en passant square before the move was made.
    int fifty;        // The fifty-move rule counter before the move was made.
    uint64_t poskey;  // The Zobrist hash key of the position before the move was made.
};

// Represents a single entry in the transposition table (hash table).
struct s_hashentry {
    uint64_t posKey;  // Position key for this entry. It's the Zobrist hash of the board position.
    int move;         // Best move for this position, found from a previous search.
    int score;        // Evaluation score for this position.
    int depth;        // The search depth at which this entry was recorded.
    int flags;        // Flags to indicate the type of score (e.g., exact, lower bound, upper bound).
};

// The transposition table structure. It stores previously analyzed positions to avoid re-calculating them.
struct s_hashtable {
    s_hashentry *pTable;  // Pointer to the dynamically allocated array of hash entries.
    int numEntries;       // The total number of entries in the hash table.
    int newWrite;         // A counter for the number of new entries written to the table.
    int overWrite;        // A counter for the number of times an existing entry was overwritten.
    int hit;              // A counter for the number of times a position was found in the table (a "hash hit").
    int cut;              // A counter for the number of times a hash hit caused a beta-cutoff in the search.
};

// Contains all control information for the current search.
struct s_searchinfo{
    int starttime;      // The time the search started, in milliseconds.
    int stoptime;       // The time the search must stop, in milliseconds.
    int depth;          // The target search depth for the current iteration.
    int timeset;        // A flag indicating if a specific search time was set.
    int movestogo;      // The number of moves remaining until the next time control.

    long nodes;         // The total number of positions (nodes) evaluated during the search.
    int infinite;       // A flag for "infinite" analysis mode (e.g., in a GUI).

    int quit;           // A flag to signal the search to quit immediately.
    int stopped;        // A flag indicating that the search has been stopped.

    float fh;           // Fail-high counter (move was better than expected). Used for move ordering heuristics.
    float fhf;          // Fail-high first counter (the first move failed high). Used for move ordering heuristics.
    int nullCut;        // A counter for cutoffs caused by the null move pruning heuristic.

    int GAME_MODE;      // The current mode of the game (e.g., Human vs Engine, Engine vs Engine).
    int POST_THINKING;  // A flag to enable or disable "pondering" or "post-thinking".
} ;

// The main board structure, containing all information about the current state of the game.
struct s_board
{
    // Board representation using 120-square "mailbox" format (10x12 board)
    int pieces[120]; // BOARD_SQ_NUM // An array representing the board. Each element stores the piece on that square. Uses a 120-square representation to easily detect off-board squares.

    uint64_t pawns[3]; // Bitboards for pawns: [0] for WHITE, [1] for BLACK, [2] for BOTH. Used for fast pawn-specific operations like pawn attacks and moves.
    int king[3];       // Stores the square of the king for each side ([0] WHITE, [1] BLACK). [2] is not used.

    int bigpce[3];     // The number of "big" pieces (not pawns) for each side ([0] WHITE, [1] BLACK).
    int majpce[3];     // The number of "major" pieces (Rook, Queen) for each side ([0] WHITE, [1] BLACK).
    int minpce[3];     // The number of "minor" pieces (Knight, Bishop) for each side ([0] WHITE, [1] BLACK).
    int material[3];   // The total material value (in centipawns) for each side ([0] WHITE, [1] BLACK).

    int side;          // The current side to move (WHITE or BLACK).
    int enpas;         // The en passant square. If no en passant capture is possible, this is set to NO_SQ.
    int fifty;         // The fifty-move rule counter. Incremented for each move that is not a pawn move or a capture.

    int ply;           // The number of half-moves (plies) made in the current search. Resets to 0 for each new search.
    int hisply;        // The total number of half-moves made throughout the entire game.
    int castleperm;    // Castling permissions, represented by a bitmask (e.g., WKCA, WQCA, BKCA, BQCA).

    uint64_t poskey;   // The Zobrist hash key for the current board position.

    int piecelist[13][10]; // A list of squares for each piece type. e.g., piecelist[wN][0] is the square of the first white knight.
    int piecenum[13];      // The number of each type of piece on the board. e.g., piecenum[wP] is the number of white pawns.
    s_undo history[2048];  // MAXGAMEMOVES // An array to store the history of moves, used to undo moves.

    s_hashtable hashtable[1]; // The transposition table for the board.

    int pvarray[64];          // MAXDEPTH // Principal Variation array, stores the best line of moves found so far.

    int searchHistory[13][120]; // BRD_SQ_NUM // A 2D array used by the history heuristic for move ordering. It scores moves based on how often they have been good in the past.
    int searchKillers[2][64];   // MAXDEPTH // The "killer move" heuristic. Stores 2 moves that caused beta-cutoffs at each ply, to be tried early in other branches.
};
