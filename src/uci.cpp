#include "defs.h"
#include "struct.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

// Global variables for UCI state
static s_board g_board;
static s_searchinfo g_info;
static bool g_uciMode = false;
static bool g_searching = false;
static string g_engineName = "Sarun";
static string g_engineAuthor = "Chess Engine Developer";

// Forward declarations
void UCI_Loop();
void UCI_ParsePosition(const string& command);
void UCI_ParseGo(const string& command);
void UCI_SendBestMove();
string UCI_GetMoveString(int move);
void UCI_PrintOptions();

// Initialize UCI mode

void UCI_Init() {
    g_uciMode = true;
    allinit();
    ResetBoard(&g_board);
    g_info.depth = 6;
    g_info.quit = false;
    g_info.stopped = false;
    g_info.bestmove = 0;
    g_info.bestscore = 0;
    // Move non-UCI info lines to stderr to avoid parser warnings
    cerr << g_engineName << " by " << g_engineAuthor << " initialized" << endl;
}
// Main UCI loop
void UCI_Loop() {
    string input;
    string token;

    // Avoid emitting non-UCI lines on stdout before commands arrive
    cerr << g_engineName << " UCI mode activated" << endl;

    while (!g_info.quit) {
        getline(cin, input);

        if (input.empty()) continue;

        stringstream ss(input);
        ss >> token;

        if (token == "uci") {
            // Identify the engine
            cout << "id name " << g_engineName << endl;
            cout << "id author " << g_engineAuthor << endl;
            UCI_PrintOptions();
            cout << "uciok" << endl;
        }
        else if (token == "isready") {
            cout << "readyok" << endl;
        }
        else if (token == "ucinewgame") {
            // Start a new game
            Parse_FEN((char*)START_FEN, &g_board);
            ClearForSearch(&g_board, &g_info);
        }
        else if (token == "position") {
            UCI_ParsePosition(input);
        }
        else if (token == "go") {
            UCI_ParseGo(input);
        }
        else if (token == "stop") {
            g_info.stopped = true;
            // Wait for search to finish
            while (g_searching) {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
            UCI_SendBestMove();
        }
        else if (token == "quit") {
            g_info.quit = true;
            break;
        }
        else if (token == "debug") {
            // Debug commands (optional)
        }
        else {
            // Unknown command - ignore
        }
    }
}

// Parse position command
void UCI_ParsePosition(const string& command) {
    stringstream ss(command);
    string token;
    ss >> token; // "position"

    ss >> token;
    if (token == "startpos") {
        Parse_FEN((char*)START_FEN, &g_board);
    }
    else if (token == "fen") {
        string fen;
        string fenPart;
        while (ss >> fenPart) {
            if (fenPart == "moves") break;
            fen += fenPart + " ";
        }
        fen = fen.substr(0, fen.length() - 1); // Remove trailing space
        Parse_FEN((char*)fen.c_str(), &g_board);
    }

    // Parse moves if present
    if (token == "moves" || (ss >> token && token == "moves")) {
        string moveStr;
        while (ss >> moveStr) {
            int move = ParseMove((char*)moveStr.c_str(), &g_board);
            if (move != FALSE) {
                MakeMove(&g_board, move);
            }
        }
    }

    // Update board state
    UpdatePieceList(&g_board);
    g_board.poskey = GeneratePosKey(&g_board);
}

// Parse go command
void UCI_ParseGo(const string& command) {
    stringstream ss(command);
    string token;
    ss >> token; // "go"

    // Reset search info
    g_info.starttime = GetTimeMs();
    g_info.depth = MAXDEPTH;
    g_info.timeset = false;
    g_info.movestogo = 40; // assume faster time controls by default
    g_info.infinite = false;
    g_info.stopped = false;
    g_searching = true;

    int movetime = -1, wtime = -1, btime = -1, winc = 0, binc = 0;

    // Parse go parameters
    while (ss >> token) {
        if (token == "depth") {
            ss >> g_info.depth;
        }
        else if (token == "movetime") {
            ss >> movetime;
        }
        else if (token == "wtime") {
            ss >> wtime;
        }
        else if (token == "btime") {
            ss >> btime;
        }
        else if (token == "winc") {
            ss >> winc;
        }
        else if (token == "binc") {
            ss >> binc;
        }
        else if (token == "movestogo") {
            ss >> g_info.movestogo;
        }
        else if (token == "infinite") {
            g_info.infinite = true;
        }
    }

    // Compute a safe think time budget if not using explicit depth
    if (movetime > 0) {
        int alloc = movetime - 30; // safety margin
        if (alloc < 50) alloc = 50;
        if (alloc > 2000) alloc = 2000; // hard cap to avoid inactivity
        g_info.stoptime = g_info.starttime + alloc;
        g_info.timeset = true;
    } else if (!g_info.infinite && (wtime >= 0 || btime >= 0)) {
        int remain = (g_board.side == WHITE ? wtime : btime);
        if (remain < 0) remain = 0;
        int inc = (g_board.side == WHITE ? winc : binc);
        int mtg = g_info.movestogo > 0 ? g_info.movestogo : 40;
        // Allocate roughly 1/(mtg+6) of remaining time plus a portion of increment
        int alloc = remain / (mtg + 6);
        alloc += (inc * 3) / 5; // use ~60% of increment
        if (alloc < 50) alloc = 50;
        if (alloc > 2000) alloc = 2000; // hard cap to avoid inactivity
        if (alloc > remain - 50) alloc = remain - 50; // leave a small safety buffer
        if (alloc < 50) alloc = 50; // clamp again if remain was tiny
        g_info.stoptime = g_info.starttime + alloc;
        g_info.timeset = true;
    } else {
        // No time controls: cap depth to something reasonable
        if (!g_info.infinite && g_info.depth == MAXDEPTH) {
            g_info.depth = 8; // safe default depth
        }
    }

    // Start search in a separate thread
    thread searchThread([&]() {
        SearchPosition(&g_board, &g_info);
        g_searching = false;
        UCI_SendBestMove();
    });

    searchThread.detach();
}

// Send best move to UCI interface
void UCI_SendBestMove() {
    if (g_info.bestmove != 0) {
        string moveStr = UCI_GetMoveString(g_info.bestmove);
        cout << "bestmove " << moveStr << endl;
    } else {
        s_movelist list;
        GenerateAllMoves(&g_board, &list);
        if (list.count > 0) {
            int bestMove = list.moves[0].move;
            string moveStr = UCI_GetMoveString(bestMove);
            cout << "bestmove " << moveStr << endl;
        } else {
            cout << "bestmove 0000" << endl;
        }
    }
}

// Convert internal move to UCI format
string UCI_GetMoveString(int move) {
    char moveStr[6];
    int from = FROMSQ(move);
    int to = TOSQ(move);
    int promoted = PROMOTED(move);
    int moveFlags = move & 0xFF0000; // Get the flag bits

    // Handle castling moves
    if (moveFlags & MFLAGCA) {
        if (to == G1) return "e1g1";  // White kingside
        if (to == C1) return "e1c1";  // White queenside
        if (to == G8) return "e8g8";  // Black kingside
        if (to == C8) return "e8c8";  // Black queenside
    }

    // Convert squares to UCI notation (e.g., e2e4)
    moveStr[0] = 'a' + FilesBrd[from];
    moveStr[1] = '1' + RanksBrd[from];
    moveStr[2] = 'a' + FilesBrd[to];
    moveStr[3] = '1' + RanksBrd[to];

    if (promoted) {
        char promChar = 'q';
        if (IsKn(promoted)) promChar = 'n';
        else if (IsRQ(promoted) && !IsBQ(promoted)) promChar = 'r';
        else if (!IsRQ(promoted) && IsBQ(promoted)) promChar = 'b';
        moveStr[4] = promChar;
        moveStr[5] = '\0';
    }
    else {
        moveStr[4] = '\0';
    }

    return string(moveStr);
}

// Print UCI options (engine-specific options)
void UCI_PrintOptions() {
    // Add engine-specific options here if needed
    // For example:
    // cout << "option name Hash type spin default 64 min 1 max 1024" << endl;
    // cout << "option name Threads type spin default 1 min 1 max 8" << endl;
}

// Get engine info
string UCI_GetEngineName() {
    return g_engineName;
}

string UCI_GetEngineAuthor() {
    return g_engineAuthor;
}

// Set engine info (for customization)
void UCI_SetEngineInfo(const string& name, const string& author) {
    g_engineName = name;
    g_engineAuthor = author;
}

// Check if UCI mode is active
bool UCI_IsActive() {
    return g_uciMode;
}
