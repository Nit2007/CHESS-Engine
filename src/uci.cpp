#include "defs.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

using namespace std;

// ================= GLOBAL STATE =================

s_board g_board;
s_searchinfo g_info;

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
    g_info.movestogo = 40;

    while (ss >> token) {
        if (token == "depth") ss >> g_info.depth;
    }

    if (g_info.depth == MAXDEPTH)
        g_info.depth = 6;

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
