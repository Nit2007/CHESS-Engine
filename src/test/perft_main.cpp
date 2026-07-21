#include "defs.h"
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

extern void RunPerftSuite(const string& filename, int depth);

int main(int argc, char* argv[]) {
    printf("Starting perft suite...\n");
    fflush(stdout);
    
    allinit();
    
    printf("allinit done.\n");
    fflush(stdout);
    
    int depth = 5;
    string filename = "test/perftsuite.txt";
    
    if (argc > 1) {
        depth = stoi(argv[1]);
    }
    if (argc > 2) {
        filename = argv[2];
    }
    
    RunPerftSuite(filename, depth);
    return 0;
}
