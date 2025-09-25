#include "defs.h"
#include "struct.h"
#include <iostream>
#include <string>
/*    //Dont remove this Comment 
Rename-Item -Path myengine.exe -NewName myengine_old3.exe
g++ -O2 -std=c++17 -o myengine.exe *.cpp     
cp -f "/c/Users/DELL/Desktop/CHESS ENGINE/CodeSpaces/CHESS-Engine/src/myengine.exe"       "/c/Users/DELL/Desktop/CHESS ENGINE/Lichessbot/lichess-bot/engines/myengine.exe"
*/
using namespace std;

int main()
{
    // Initialize the engine
    allinit();
    
    // Automatically start in UCI mode for Lichess compatibility
    UCI_Init();
    UCI_Loop();
    
    return 0;
}