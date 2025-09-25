#include "defs.h"
#include "struct.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    cout << "Chess Engine - Choose Mode:" << endl;
    cout << "1. Console Mode (for testing)" << endl;
    cout << "2. UCI Mode (for Lichess, ChessBase)" << endl;
    cout << "3. xBoard Mode (for xBoard/WinBoard)" << endl;
    cout << "Enter choice (1-3): ";

    int choice;
    cin >> choice;
    cin.ignore(); // Clear newline

    // Initialize the engine
    allinit();

    if (choice == 2) {
        cout << "Starting Chess Engine in UCI mode..." << endl;
        UCI_Init();
        UCI_Loop();
    }
    else if (choice == 3) {
        cout << "Starting Chess Engine in xBoard mode..." << endl;
        XBoard_Init();
        XBoard_Loop();
    }
    else {
        // Default console mode for testing
        cout << "Starting Chess Engine in Console mode..." << endl;
        s_board board;
        Parse_FEN((char*)FEN_2, &board);
        PrintBoard(&board);
        s_movelist list;
        GenerateAllMoves(&board, &list);
        PrintMoveList(&list);
        s_searchinfo* info = new s_searchinfo;
        info->depth = 6;
        ClearForSearch(&board, info);
        SearchPosition(&board, info);
        cout << endl << "*************end*************" << endl;

        delete info;
    }

    return 0;
}

/*#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
    s_board board;
    Parse_FEN((char*)FEN_2,&board);
    PrintBoard(&board);
    s_movelist list;
    GenerateAllMoves(&board,&list);
    PrintMoveList(&list);
    s_searchinfo* info = new s_searchinfo;
    info->depth = 6; // Set search depth
    ClearForSearch(&board,info);
    SearchPosition(&board,info);
    cout<<endl<<"*************end*************"<<endl<<endl<<endl;
    
   // delete info;
    return 0;
}
*/