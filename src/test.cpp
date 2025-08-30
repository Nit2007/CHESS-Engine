#include "defs.h"
#include "struct.h"


void SimpleTestLoop() {
    allinit();
    s_board board;
    Parse_FEN((char*)START_FEN, &board);
    
    cout << "=== SIMPLE CHESS MOVE TESTER ===" << endl;
    cout << "Enter moves like: e2e4, g1f3, e7e8q" << endl;
    cout << "'t' = takeback move" << endl;
    cout << "'q' = quit" << endl;
    cout << "=================================" << endl;
    
    PrintBoard(&board);
    
    char input[256];
    vector<int> moveHistory;
    
    while (true) {
        cout << "\n> ";
        cin >> input;
        
        // Quit command
        if (input[0] == 'q') {
            cout << "Goodbye!" << endl;
            break;
        }
        
        // Takeback command
        if (input[0] == 't') {
            if (moveHistory.empty()) {
                cout << "No moves to take back!" << endl;
            } else {
                TakeMove(&board);
                moveHistory.pop_back();
                cout << "Move taken back!" << endl;
                PrintBoard(&board);
            }
            continue;
        }
        
        // Try to parse and make move
        int move = ParseMove(input, &board);
        
        if (move == FALSE) {
            cout << "Invalid move: " << input << endl;
            cout << "Try format: e2e4, g1f3, etc." << endl;
        } else {
            if (MakeMove(&board, move)) {
                moveHistory.push_back(move);
                cout << "Move made: " << PrMove(move) << endl;
                PrintBoard(&board);
                
                // Check for game over
                s_movelist nextMoves;
                GenerateAllMoves(&board, &nextMoves);
                if (nextMoves.count == 0) {
                    if (SqAttacked(board.king[board.side], board.side^1, &board)) {
                        cout << "\n*** CHECKMATE! Game Over ***" << endl;
                    } else {
                        cout << "\n*** STALEMATE! Game Over ***" << endl;
                    }
                }
            } else {
                cout << "Illegal move (king would be in check): " << input << endl;
            }
        }
    }
}



// Test positions for various scenarios
#define FOOLS_MATE_FEN "rnbqkb1r/pppp1ppp/5n2/4p3/2B1P3/8/PPPP1PpP/RNBQK1NR w KQkq - 0 4"
#define STALEMATE_FEN "5k2/5P2/5K2/8/8/8/8/8 b - - 0 1"
#define CHECK_TEST_FEN "rnbqk2r/pppp1ppp/5n2/2b1p3/2B1P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 0 4"

struct GameState {
    vector<s_board> positions;
    vector<int> moves;
    int currentPos;
    
    GameState() : currentPos(0) {}
    
    void addPosition(const s_board& board, int move = 0) {
        // Remove any positions ahead of current if we're not at the end
        if (currentPos < static_cast<int>(positions.size())) {
            positions.erase(positions.begin() + currentPos + 1, positions.end());
            moves.erase(moves.begin() + currentPos, moves.end());
        }
        
        positions.push_back(board);
        if (move != 0) {
            moves.push_back(move);
        }
        currentPos = static_cast<int>(positions.size()) - 1;
    }
    
    bool canGoBack() const {
        return currentPos > 0;
    }
    
    bool canGoForward() const {
        return currentPos < static_cast<int>(positions.size()) - 1;
    }
    
    void goBack() {
        if (canGoBack()) {
            currentPos--;
        }
    }
    
    void goForward() {
        if (canGoForward()) {
            currentPos++;
        }
    }
    
    s_board& getCurrentBoard() {
        return positions[currentPos];
    }
    
    void reset() {
        positions.clear();
        moves.clear();
        currentPos = 0;
    }
};



void showMaterialEvaluation(const s_board* pos) {
    cout << "\n=== MATERIAL EVALUATION ===" << endl;
    cout << "White Material: " << pos->material[WHITE] << " centipawns" << endl;
    cout << "Black Material: " << pos->material[BLACK] << " centipawns" << endl;
    cout << "Material Balance: " << (pos->material[WHITE] - pos->material[BLACK]) << " centipawns" << endl;
    
    cout << "\nPiece Count Summary:" << endl;
    cout << "White: " << pos->piecenum[WP] << "P " << pos->piecenum[WN] << "N " 
         << pos->piecenum[WB] << "B " << pos->piecenum[WR] << "R " 
         << pos->piecenum[WQ] << "Q " << pos->piecenum[WK] << "K" << endl;
    cout << "Black: " << pos->piecenum[BP] << "p " << pos->piecenum[BN] << "n " 
         << pos->piecenum[BB] << "b " << pos->piecenum[BR] << "r " 
         << pos->piecenum[BQ] << "q " << pos->piecenum[BK] << "k" << endl;
         
    cout << "\nOther Metrics:" << endl;
    cout << "Big Pieces (W/B): " << pos->bigpce[WHITE] << "/" << pos->bigpce[BLACK] << endl;
    cout << "Major Pieces (W/B): " << pos->majpce[WHITE] << "/" << pos->majpce[BLACK] << endl;
    cout << "Minor Pieces (W/B): " << pos->minpce[WHITE] << "/" << pos->minpce[BLACK] << endl;
}

void showCheckStatus(const s_board* pos) {
    cout << "\n=== CHECK STATUS ===" << endl;
    bool whiteInCheck = SqAttacked(pos->king[WHITE], BLACK, pos);
    bool blackInCheck = SqAttacked(pos->king[BLACK], WHITE, pos);
    
    cout << "White King (" << PrSq(pos->king[WHITE]) << "): " 
         << (whiteInCheck ? "IN CHECK" : "Safe") << endl;
    cout << "Black King (" << PrSq(pos->king[BLACK]) << "): " 
         << (blackInCheck ? "IN CHECK" : "Safe") << endl;
         
    if (whiteInCheck || blackInCheck) {
        cout << "Current side to move: " << (pos->side == WHITE ? "WHITE" : "BLACK") 
             << (((pos->side == WHITE && whiteInCheck) || (pos->side == BLACK && blackInCheck)) 
                 ? " (MUST ESCAPE CHECK!)" : "") << endl;
    }
}

void analyzeMateStatus(const s_board* pos) {
    cout << "\n=== COMPREHENSIVE MATE ANALYSIS ===" << endl;
    
    // Generate all legal moves for current side
    s_movelist list;
    GenerateAllMoves(pos, &list);
    
    // Test each move to see if it's legal
    int legalMoves = 0;
    vector<int> validMoves;
    
    for (int i = 0; i < list.count; i++) {
        s_board tempBoard = *pos;
        if (MakeMove(&tempBoard, list.moves[i].move)) {
            legalMoves++;
            validMoves.push_back(list.moves[i].move);
        }
    }
    
    bool inCheck = SqAttacked(pos->king[pos->side], pos->side^1, pos);
    
    cout << "Side to move: " << (pos->side == WHITE ? "WHITE" : "BLACK") << endl;
    cout << "In check: " << (inCheck ? "YES" : "NO") << endl;
    cout << "Legal moves available: " << legalMoves << endl;
    
    if (legalMoves == 0) {
        if (inCheck) {
            cout << "\n*** CHECKMATE! " << (pos->side == WHITE ? "BLACK" : "WHITE") 
                 << " WINS! ***" << endl;
        } else {
            cout << "\n*** STALEMATE! DRAW! ***" << endl;
        }
    } else {
        cout << "\nGame continues..." << endl;
        if (legalMoves <= 5 && legalMoves > 0) {
            cout << "Legal moves: ";
            for (int move : validMoves) {
                cout << PrMove(move) << " ";
            }
            cout << endl;
        }
    }
}

void showCurrentFEN(const s_board* pos) {
    cout << "\n=== CURRENT POSITION FEN ===" << endl;
    // This would require implementing a board-to-FEN function
    // For now, show the position key and basic info
    cout << "Position Hash Key: " << pos->poskey << endl;
    cout << "Side to move: " << (pos->side == WHITE ? "w" : "b") << endl;
    cout << "Castling rights: " 
         << ((pos->castleperm & WKCA) ? 'K' : '-')
         << ((pos->castleperm & WQCA) ? 'Q' : '-')
         << ((pos->castleperm & BKCA) ? 'k' : '-')
         << ((pos->castleperm & BQCA) ? 'q' : '-') << endl;
    cout << "En passant: " << (pos->enpas == NO_SQ ? "none" : PrSq(pos->enpas)) << endl;
    cout << "Fifty move rule: " << pos->fifty << endl;
    cout << "Game ply: " << pos->hisply << endl;
}

void showNavigationStatus(const GameState& gameState) {
    cout << "\n=== NAVIGATION STATUS ===" << endl;
    cout << "Position: " << (gameState.currentPos + 1) 
         << "/" << gameState.positions.size() << endl;
    cout << "Can go back: " << (gameState.canGoBack() ? "Yes" : "No") << endl;
    cout << "Can go forward: " << (gameState.canGoForward() ? "Yes" : "No") << endl;
    
    if (gameState.currentPos > 0 && 
        gameState.currentPos <= static_cast<int>(gameState.moves.size())) {
        cout << "Last move: " 
             << PrMove(gameState.moves[gameState.currentPos - 1]) << endl;
    }
}



void showHelp() {
    cout << "\n=== ENHANCED CHESS ENGINE TESTER ===" << endl;
    cout << "\n📝 MOVE COMMANDS:" << endl;
    cout << "  Enter moves: e2e4, g1f3, e7e8q, etc." << endl;
    cout << "  t = Take back last move" << endl;
    cout << "\n⬅️➡️ NAVIGATION COMMANDS:" << endl;
    cout << "  b = Move backward through game history" << endl;
    cout << "  f = Move forward through game history" << endl;
    cout << "\n🔍 ANALYSIS COMMANDS:" << endl;
    cout << "  moves = Show all legal moves" << endl;
    cout << "  eval = Material evaluation and position assessment" << endl;
    cout << "  check = Show check status for both kings" << endl;
    cout << "  mate = Comprehensive checkmate/stalemate analysis" << endl;
    cout << "\n🎯 TEST POSITIONS:" << endl;
    cout << "  testmate = Load Fool's Mate position" << endl;
    cout << "  teststale = Load stalemate position" << endl;
    cout << "  testcheck = Load check position" << endl;
    cout << "\n🛠️ UTILITY FEATURES:" << endl;
    cout << "  fen = Show current position info" << endl;
    cout << "  reset = Start new game" << endl;
    cout << "  show = Redisplay current board" << endl;
    cout << "  nav = Show navigation status" << endl;
    cout << "  help = Show this help" << endl;
    cout << "  q = Quit" << endl;
    cout << "=================================" << endl;
}

void ComplexTestLoop() {
    allinit();
    GameState gameState;
    s_board board;
    Parse_FEN((char*)START_FEN, &board);
    gameState.addPosition(board);
    
    showHelp();
    PrintBoard(&board);
    
    char input[256];
    
    while (true) {
        cout << "\n> ";
        cin >> input;
        
        // Convert input to lowercase for command matching
        string cmd = input;
        for (auto& c : cmd) c = tolower(c);
        
        // Quit command
        if (cmd == "q" || cmd == "quit") {
            cout << "Goodbye!" << endl;
            break;
        }
        
        // Help command
        else if (cmd == "help" || cmd == "h") {
            showHelp();
        }
        
        // Navigation commands
        else if (cmd == "b" || cmd == "back") {
            if (gameState.canGoBack()) {
                gameState.goBack();
                board = gameState.getCurrentBoard();
                cout << "Moved backward in history!" << endl;
                PrintBoard(&board);
                showNavigationStatus(gameState);
            } else {
                cout << "Cannot go back - already at start!" << endl;
            }
        }
        
        else if (cmd == "f" || cmd == "forward") {
            if (gameState.canGoForward()) {
                gameState.goForward();
                board = gameState.getCurrentBoard();
                cout << "Moved forward in history!" << endl;
                PrintBoard(&board);
                showNavigationStatus(gameState);
            } else {
                cout << "Cannot go forward - already at latest position!" << endl;
            }
        }
        
        // Analysis commands
        else if (cmd == "moves") {
            s_movelist moveList;
            GenerateAllMoves(&board, &moveList);
            
            // Filter only legal moves
            s_movelist legalMoves;
            legalMoves.count = 0;
            
            for (int i = 0; i < moveList.count; i++) {
                s_board tempBoard = board;
                if (MakeMove(&tempBoard, moveList.moves[i].move)) {
                    legalMoves.moves[legalMoves.count++] = moveList.moves[i];
                }
            }
            
            cout << "\n=== LEGAL MOVES (" << legalMoves.count << ") ===" << endl;
            PrintMoveList(&legalMoves);
        }
        
        else if (cmd == "eval") {
            showMaterialEvaluation(&board);
        }
        
        else if (cmd == "check") {
            showCheckStatus(&board);
        }
        
        else if (cmd == "mate") {
            analyzeMateStatus(&board);
        }
        
        // Test positions
        else if (cmd == "testmate") {
            Parse_FEN((char*)FOOLS_MATE_FEN, &board);
            gameState.reset();
            gameState.addPosition(board);
            cout << "Loaded Fool's Mate position!" << endl;
            PrintBoard(&board);
            analyzeMateStatus(&board);
        }
        
        else if (cmd == "teststale") {
            Parse_FEN((char*)STALEMATE_FEN, &board);
            gameState.reset();
            gameState.addPosition(board);
            cout << "Loaded stalemate test position!" << endl;
            PrintBoard(&board);
            analyzeMateStatus(&board);
        }
        
        else if (cmd == "testcheck") {
            Parse_FEN((char*)CHECK_TEST_FEN, &board);
            gameState.reset();
            gameState.addPosition(board);
            cout << "Loaded check test position!" << endl;
            PrintBoard(&board);
            showCheckStatus(&board);
        }
        
        // Utility commands
        else if (cmd == "fen") {
            showCurrentFEN(&board);
        }
        
        else if (cmd == "reset") {
            Parse_FEN((char*)START_FEN, &board);
            gameState.reset();
            gameState.addPosition(board);
            cout << "Game reset to starting position!" << endl;
            PrintBoard(&board);
        }
        
        else if (cmd == "show") {
            PrintBoard(&board);
        }
        
        else if (cmd == "nav") {
            showNavigationStatus(gameState);
        }
        
        // Takeback command
        else if (cmd == "t" || cmd == "takeback") {
            if (gameState.canGoBack()) {
                gameState.goBack();
                board = gameState.getCurrentBoard();
                cout << "Move taken back!" << endl;
                PrintBoard(&board);
            } else {
                cout << "No moves to take back!" << endl;
            }
        }
        
        // Try to parse as a move
        else {
            int move = ParseMove(input, &board);
            
            if (move == FALSE) {
                cout << "Invalid command or move: " << input << endl;
                cout << "Type 'help' for available commands." << endl;
            } else {
                if (MakeMove(&board, move)) {
                    gameState.addPosition(board, move);
                    cout << "Move made: " << PrMove(move) << endl;
                    PrintBoard(&board);
                    
                    // Auto-analyze game state after each move
                    s_movelist nextMoves;
                    GenerateAllMoves(&board, &nextMoves);
                    
                    int legalCount = 0;
                    for (int i = 0; i < nextMoves.count; i++) {
                        s_board tempBoard = board;
                        if (MakeMove(&tempBoard, nextMoves.moves[i].move)) {
                            legalCount++;
                        }
                    }
                    
                    if (legalCount == 0) {
                        analyzeMateStatus(&board);
                    } else {
                        bool inCheck = SqAttacked(board.king[board.side], board.side^1, &board);
                        if (inCheck) {
                            cout << "\n" << (board.side == WHITE ? "WHITE" : "BLACK") 
                                 << " is in CHECK!" << endl;
                        }
                    }
                    
                } else {
                    cout << "Illegal move (king would be in check): " << input << endl;
                }
            }
        }
    }
}
