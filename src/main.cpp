#include "defs.h"
#include "struct.h"
#include <iostream>
#include <string>

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