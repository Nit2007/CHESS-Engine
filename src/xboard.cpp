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

// Global variables for xBoard state
static s_board g_xboard;
static s_searchinfo g_xinfo;
static bool g_xboardMode = false;
static bool g_forceMode = false;
static bool g_xsearching = false;
static string g_xengineName = "ChessEngine";
static string g_xengineAuthor = "Chess Engine Developer";

// Forward declarations
void XBoard_Loop();
void XBoard_ParseCommand(const string& command);
void XBoard_ParseMove(const string& moveStr);
void XBoard_SendMove(int move);
void XBoard_PrintResult();
string XBoard_GetMoveString(int move);

// Initialize xBoard mode
void XBoard_Init() {
    g_xboardMode = true;
    allinit();
    ResetBoard(&g_xboard);
    g_xinfo.depth = 6;
    g_xinfo.quit = false;
    g_xinfo.stopped = false;
    cout << "feature done=0" << endl; // xBoard protocol initialization
}

// Main xBoard loop
void XBoard_Loop() {
    string input;

    cout << g_xengineName << " xBoard mode activated" << endl;

    while (!g_xinfo.quit) {
        getline(cin, input);

        if (input.empty()) continue;

        XBoard_ParseCommand(input);
    }
}

// Parse xBoard commands
void XBoard_ParseCommand(const string& command) {
    stringstream ss(command);
    string token;
    ss >> token;

    if (token == "xboard") {
        // Enter xBoard mode
        cout << "feature done=0" << endl;
    }
    else if (token == "protover") {
        int version;
        ss >> version;
        // Send supported features
        cout << "feature myname=\"" << g_xengineName << "\"" << endl;
        cout << "feature playother=1" << endl;
        cout << "feature san=0" << endl;
        cout << "feature usermove=1" << endl;
        cout << "feature time=1" << endl;
        cout << "feature sigint=0" << endl;
        cout << "feature sigterm=0" << endl;
        cout << "feature done=1" << endl;
    }
    else if (token == "accepted" || token == "rejected") {
        // Feature negotiation responses - ignore for now
    }
    else if (token == "new") {
        // Start a new game
        Parse_FEN((char*)START_FEN, &g_xboard);
        g_forceMode = false;
        cout << "telluser New game started" << endl;
    }
    else if (token == "force") {
        // Enter force mode (engine won't move automatically)
        g_forceMode = true;
    }
    else if (token == "go") {
        // Start thinking
        if (!g_forceMode) {
            g_xsearching = true;
            g_xinfo.starttime = GetTimeMs();
            g_xinfo.depth = MAXDEPTH;
            g_xinfo.stopped = false;

            // Simple time management - use remaining time
            if (g_xboard.side == WHITE) {
                g_xinfo.stoptime = g_xinfo.starttime + 1000; // 1 second for now
            } else {
                g_xinfo.stoptime = g_xinfo.starttime + 1000; // 1 second for now
            }

            thread searchThread([&]() {
                SearchPosition(&g_xboard, &g_xinfo);
                g_xsearching = false;

                // Get best move and send it
                s_movelist list;
                GenerateAllMoves(&g_xboard, &list);

                if (list.count > 0) {
                    int bestMove = list.moves[0].move; // Simplified - pick first move
                    XBoard_SendMove(bestMove);
                }
            });

            searchThread.detach();
        }
    }
    else if (token == "move") {
        string moveStr;
        ss >> moveStr;
        XBoard_ParseMove(moveStr);
    }
    else if (token == "usermove") {
        string moveStr;
        ss >> moveStr;
        XBoard_ParseMove(moveStr);
    }
    else if (token == "time") {
        int timeLeft;
        ss >> timeLeft;
        // Update time information (simplified)
    }
    else if (token == "otim") {
        int opponentTime;
        ss >> opponentTime;
        // Opponent time (not used in simple implementation)
    }
    else if (token == "quit") {
        g_xinfo.quit = true;
    }
    else if (token == "result") {
        // Game result
        string result;
        getline(ss, result);
        cout << "telluser Game ended: " << result << endl;
    }
    else if (token == "setboard") {
        // Set board position (simplified - just reset to start)
        Parse_FEN((char*)START_FEN, &g_xboard);
    }
    else if (token == "edit") {
        // Edit mode (not fully implemented)
        cout << "telluser Edit mode not supported" << endl;
    }
    else if (token == "hint") {
        // Give a hint (simplified)
        s_movelist list;
        GenerateAllMoves(&g_xboard, &list);

        if (list.count > 0) {
            int hintMove = list.moves[0].move;
            string moveStr = XBoard_GetMoveString(hintMove);
            cout << "Hint: " << moveStr << endl;
        }
    }
    else if (token == "undo") {
        // Take back last move
        if (g_xboard.hisply > 0) {
            TakeMove(&g_xboard);
            cout << "telluser Move undone" << endl;
        }
    }
    else if (token == "remove") {
        // Take back two moves
        if (g_xboard.hisply > 0) {
            TakeMove(&g_xboard);
            if (g_xboard.hisply > 0) {
                TakeMove(&g_xboard);
            }
            cout << "telluser Two moves removed" << endl;
        }
    }
    else if (token == "hard") {
        // Enable permanent brain (pondering)
        cout << "telluser Pondering enabled" << endl;
    }
    else if (token == "easy") {
        // Disable permanent brain
        cout << "telluser Pondering disabled" << endl;
    }
    else if (token == "post") {
        // Enable thinking output
        cout << "telluser Thinking output enabled" << endl;
    }
    else if (token == "nopost") {
        // Disable thinking output
        cout << "telluser Thinking output disabled" << endl;
    }
    else if (token == "analyze") {
        // Enter analyze mode
        cout << "telluser Analyze mode not fully implemented" << endl;
    }
    else if (token == "exit") {
        // Exit analyze mode
    }
    else if (token == "name") {
        // Set opponent name (ignore)
    }
    else if (token == "rating") {
        // Rating information (ignore)
    }
    else if (token == "computer") {
        // Computer opponent (ignore)
    }
    else {
        // Unknown command - try to parse as move
        XBoard_ParseMove(token);
    }
}

// Parse and make a move
void XBoard_ParseMove(const string& moveStr) {
    int move = ParseMove((char*)moveStr.c_str(), &g_xboard);

    if (move != FALSE) {
        // Make the move
        if (MakeMove(&g_xboard, move)) {
            // Move was successful
            if (!g_forceMode) {
                // If not in force mode, start thinking for next move
                g_xsearching = true;
                g_xinfo.starttime = GetTimeMs();
                g_xinfo.depth = MAXDEPTH;
                g_xinfo.stopped = false;

                thread searchThread([&]() {
                    SearchPosition(&g_xboard, &g_xinfo);
                    g_xsearching = false;

                    // Get best move and send it
                    s_movelist list;
                    GenerateAllMoves(&g_xboard, &list);

                    if (list.count > 0) {
                        int bestMove = list.moves[0].move;
                        XBoard_SendMove(bestMove);
                    }
                });

                searchThread.detach();
            }
        }
        else {
            cout << "Illegal move: " << moveStr << endl;
        }
    }
    else {
        cout << "Error (unknown command): " << moveStr << endl;
    }
}

// Send a move to xBoard
void XBoard_SendMove(int move) {
    string moveStr = XBoard_GetMoveString(move);
    cout << "move " << moveStr << endl;

    // Make the move on the board
    MakeMove(&g_xboard, move);

    // Check for game end conditions
    XBoard_PrintResult();
}

// Convert internal move to xBoard format
string XBoard_GetMoveString(int move) {
    char moveStr[6];
    int from = FROMSQ(move);
    int to = TOSQ(move);
    int promoted = PROMOTED(move);

    // Convert squares to xBoard notation (e.g., e2e4)
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

// Print game result
void XBoard_PrintResult() {
    s_movelist list;
    GenerateAllMoves(&g_xboard, &list);

    if (list.count == 0) {
        if (g_xboard.side == WHITE) {
            cout << "0-1 {Black wins}" << endl;
        } else {
            cout << "1-0 {White wins}" << endl;
        }
    }
    else {
        // Check for draw conditions (simplified)
        if (g_xboard.fifty >= 50) {
            cout << "1/2-1/2 {Fifty move rule}" << endl;
        }
        else if (IsRepetition(&g_xboard)) {
            cout << "1/2-1/2 {Threefold repetition}" << endl;
        }
    }
}

// Get engine info
string XBoard_GetEngineName() {
    return g_xengineName;
}

string XBoard_GetEngineAuthor() {
    return g_xengineAuthor;
}

// Set engine info (for customization)
void XBoard_SetEngineInfo(const string& name, const string& author) {
    g_xengineName = name;
    g_xengineAuthor = author;
}

// Check if xBoard mode is active
bool XBoard_IsActive() {
    return g_xboardMode;
}
