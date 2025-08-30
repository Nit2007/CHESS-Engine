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
