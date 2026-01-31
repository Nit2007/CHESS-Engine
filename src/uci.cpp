#include "defs.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>

using namespace std;

// ================= GLOBAL STATE =================

s_board g_board;
s_searchinfo g_info;

// Opening book globals
s_poly_book_entry* g_book = nullptr;
uint64_t g_book_entries = 0;

bool g_uciMode = false;

string g_engineName   = "Sarun";
string g_engineAuthor = "Chess Engine Developer";

// ================= FORWARD DECLS =================

void UCI_Loop();
void UCI_ParsePosition(const string& command);
void UCI_ParseGo(const string& command);
void UCI_SendBestMove();
string UCI_MoveToString(int move);


// ================= INIT ==========================

void UCI_Init() {
    g_uciMode = true;

    allinit();
    ResetBoard(&g_board);
    Parse_FEN((char*)START_FEN, &g_board);

    InitHashTable(g_board.hashtable, 64);

    g_info.quit = false;
    g_info.stopped = false;
    g_info.timeset = false;
    g_info.depth = 6;
    g_info.bestmove = 0;

    cerr << g_engineName << " by " << g_engineAuthor << " initialized\n";
    // seed RNG for book weighted selection and load book
    srand((unsigned)time(NULL));
    g_book = InitPolyBook(g_book_entries);
    if (g_book != nullptr) cerr << "Opening book loaded: " << g_book_entries << " entries\n";
}

// ================= UCI LOOP ======================

void UCI_Loop() {
    string line, token;

    cerr << g_engineName << " UCI mode activated\n";

    while (!g_info.quit) {

        // CRITICAL: lichess closes stdin → exit safely
        if (!getline(cin, line)) {
            g_info.quit = true;
            break;
        }

        if (line.empty()) continue;

        stringstream ss(line);
        ss >> token;

        if (token == "uci") {
            cout << "id name " << g_engineName << endl;
            cout << "id author " << g_engineAuthor << endl;
            cout << "uciok" << endl << flush;
        }
        else if (token == "isready") {
            cout << "readyok" << endl << flush;
        }
        else if (token == "ucinewgame") {
            ResetBoard(&g_board);
            Parse_FEN((char*)START_FEN, &g_board);
            ClearForSearch(&g_board, &g_info);
        }
        else if (token == "position") {
            UCI_ParsePosition(line);
        }
        else if (token == "go") {
            UCI_ParseGo(line);
        }
        else if (token == "stop") {
            g_info.stopped = true;
        }
        else if (token == "quit") {
            g_info.quit = true;
            break;
        }
    }

    // free opening book on exit
    if (g_book) CleanPolyBook(g_book);
}

// ================= POSITION ======================

void UCI_ParsePosition(const string& command) {
    stringstream ss(command);
    string token;

    ss >> token; // position
    ss >> token;

    if (token == "startpos") {
        Parse_FEN((char*)START_FEN, &g_board);
        if (ss >> token) {
            if (token != "moves") return;
        } else {
            return;
        }
    } else if (token == "fen") {
        string fen, part;
        int fenParts = 0;
        while (fenParts < 6 && (ss >> part)) {
            fen += part + " ";
            fenParts++;
        }
        if (!fen.empty()) fen.pop_back();

        char buf[256];
        strncpy(buf, fen.c_str(), 255);
        buf[255] = 0;

        Parse_FEN(buf, &g_board);

        if (ss >> token) {
            if (token != "moves") return;
        } else {
            return;
        }
    } else if (token == "moves") {
        Parse_FEN((char*)START_FEN, &g_board);
    } else {
        return;
    }

    string moveStr;
    while (ss >> moveStr) {
        int move = ParseMove((char*)moveStr.c_str(), &g_board);
        if (move != FALSE) {
            MakeMove(&g_board, move);
        }
    }

    ASSERT(CheckBoard(&g_board));
}

// ================= GO ============================

void UCI_ParseGo(const string& command) {
    stringstream ss(command);
    string token;

    ss >> token; // go

    g_info.starttime = GetTimeMs();
    g_info.stopped = false;
    g_info.timeset = false;
    g_info.infinite = false;
    g_info.depth = MAXDEPTH;
    g_info.movestogo = 30;

    int depth = -1;
    int movetime = -1;
    int wtime = -1, btime = -1;
    int winc = 0, binc = 0;
    int movestogo = 30;
    bool infinite = false;

    while (ss >> token) {
        if (token == "depth") {
            ss >> depth;
        } else if (token == "movetime") {
            ss >> movetime;
        } else if (token == "wtime") {
            ss >> wtime;
        } else if (token == "btime") {
            ss >> btime;
        } else if (token == "winc") {
            ss >> winc;
        } else if (token == "binc") {
            ss >> binc;
        } else if (token == "movestogo") {
            ss >> movestogo;
        } else if (token == "infinite") {
            infinite = true;
        }
    }

    if (movestogo > 0) {
        g_info.movestogo = movestogo;
    } else {
        g_info.movestogo = 30;
    }

    if (depth != -1) {
        g_info.depth = depth;
    } else {
        g_info.depth = 6;
    }

    if (infinite) {
        g_info.infinite = true;
        g_info.timeset = false;
    } else {
        int timeForMove = -1;

        if (movetime != -1) {
            timeForMove = movetime;
        } else if (wtime != -1 && btime != -1) {
            const int remaining = (g_board.side == WHITE) ? wtime : btime;
            const int inc = (g_board.side == WHITE) ? winc : binc;
            int movesToGo = g_info.movestogo;
            if (movesToGo < 1) movesToGo = 30;

            timeForMove = (remaining / movesToGo) + inc;

            timeForMove = (timeForMove * 8) / 10;
            if (timeForMove < 50) timeForMove = 50;
        }

        if (timeForMove != -1) {
            const int safetyBufferMs = 50;
            g_info.timeset = true;
            if (timeForMove > safetyBufferMs) {
                timeForMove -= safetyBufferMs;
            }
            g_info.stoptime = g_info.starttime + timeForMove;

            if (depth == -1) {
                if (timeForMove < 200) {
                    g_info.depth = 2;
                } else if (timeForMove < 500) {
                    g_info.depth = 3;
                } else if (timeForMove < 1200) {
                    g_info.depth = 4;
                } else if (timeForMove < 2500) {
                    g_info.depth = 5;
                } else {
                    g_info.depth = 6;
                }
            }
        }
    }

//     // Opening override (safe + legal): enforce 1. e4 and ...g6
//     // Only applies to the very first moves; otherwise normal search.
//     {
//         if (g_board.hisply == 0 && g_board.side == WHITE) {
//             int mv = ParseMove((char*)"e2e4", &g_board);
//             if (mv != FALSE) {
//                 if (MakeMove(&g_board, mv)) {
//                     TakeMove(&g_board);
//                     g_info.bestmove = mv;
//                     UCI_SendBestMove();
//                     return;
//                 }
//             }
//         }

//         if (g_board.hisply == 1 && g_board.side == BLACK) {
//         int mv = ParseMove((char*)"b7b6", &g_board);
//         if (mv != FALSE) {
//             if (MakeMove(&g_board, mv)) {
//                 TakeMove(&g_board);
//                 g_info.bestmove = mv;
//                 UCI_SendBestMove();
//                 return;
//             }
//         }
// }
//     }

    // Try book move before searching
    if (g_book != nullptr && g_book_entries > 0) {
        int bmove = GetBookMove(&g_board, g_book, g_book_entries);
        if (bmove != 0) {
            cerr << "Book candidate: " << UCI_MoveToString(bmove) << "\n";
            if (MakeMove(&g_board, bmove)) {
                TakeMove(&g_board);
                g_info.bestmove = bmove;
                cerr << "Using book move: " << UCI_MoveToString(bmove) << "\n";
                UCI_SendBestMove();
                return;
            } else {
                cerr << "Book move illegal in current position: " << UCI_MoveToString(bmove) << "\n";
            }
        }
    }

    SearchPosition(&g_board, &g_info);
    UCI_SendBestMove();
}

// ================= BEST MOVE =====================

void UCI_SendBestMove() {
    int move = g_info.bestmove;

    if (move == 0) {
        s_movelist list;
        GenerateAllMoves(&g_board, &list);

        for (int i = 0; i < list.count; i++) {
            if (MakeMove(&g_board, list.moves[i].move)) {
                TakeMove(&g_board);
                move = list.moves[i].move;
                break;
            }
        }
    }

    if (move == 0) {
        cout << "bestmove 0000" << endl;
        return;
    }

    cout << "bestmove " << UCI_MoveToString(move) << endl;
}

// ================= MOVE STRING ===================

string UCI_MoveToString(int move) {
    char buf[6];

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int promo = PROMOTED(move);

    buf[0] = 'a' + FilesBrd[from];
    buf[1] = '1' + RanksBrd[from];
    buf[2] = 'a' + FilesBrd[to];
    buf[3] = '1' + RanksBrd[to];

    if (promo) {
        if (promo == WQ || promo == BQ) buf[4] = 'q';
        else if (promo == WR || promo == BR) buf[4] = 'r';
        else if (promo == WB || promo == BB) buf[4] = 'b';
        else buf[4] = 'n';
        buf[5] = 0;
    } else {
        buf[4] = 0;
    }

    return string(buf);
}
