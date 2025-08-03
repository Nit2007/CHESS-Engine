#include "defs.h"
#include "struct.h"


int main()
{
    allinit();
     printf("Testing CheckBoard function...\n\n");
    
    // TEST 1: Valid starting position (should PASS)
    printf("Test 1: Valid starting position\n");
    ParseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);
    UpdatePieceList(&pos);
    if(CheckBoard(&pos)) {
        printf("✓ PASSED: Starting position is valid\n");
    } else {
        printf("✗ FAILED: Starting position should be valid\n");
    }
    
    // TEST 2: Corrupt piece count (should FAIL)
    printf("\nTest 2: Corrupted piece count\n");
    ParseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);
    UpdatePieceList(&pos);
    pos.piecenum[WP] = 7; // Should be 8, corrupt it
    printf("Corrupting white pawn count from 8 to 7...\n");
    if(!CheckBoard(&pos)) {
        printf("✓ PASSED: Correctly detected corrupted piece count\n");
    } else {
        printf("✗ FAILED: Should have detected corruption\n");
    }
    
    // TEST 3: Mismatched piece on board (should FAIL)
    printf("\nTest 3: Wrong piece on square\n");
    ParseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);
    UpdatePieceList(&pos);
    pos.pieces[A1] = WQ; // Should be WR, corrupt it
    printf("Changing piece at A1 from WR to WQ...\n");
    if(!CheckBoard(&pos)) {
        printf("✓ PASSED: Correctly detected wrong piece\n");
    } else {
        printf("✗ FAILED: Should have detected wrong piece\n");
    }
    
    // TEST 4: Wrong material count (should FAIL)
    printf("\nTest 4: Wrong material count\n");
    ParseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);
    UpdatePieceList(&pos);
    pos.material[WHITE] += 100; // Add extra material
    printf("Adding 100 to white material count...\n");
    if(!CheckBoard(&pos)) {
        printf("✓ PASSED: Correctly detected wrong material\n");
    } else {
        printf("✗ FAILED: Should have detected wrong material\n");
    }
    
    // TEST 5: Invalid side to move (should FAIL)
    printf("\nTest 5: Invalid side to move\n");
    ParseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);
    UpdatePieceList(&pos);
    pos.side = 2; // Invalid side (should be WHITE=0 or BLACK=1)
    printf("Setting side to invalid value 2...\n");
    if(!CheckBoard(&pos)) {
        printf("✓ PASSED: Correctly detected invalid side\n");
    } else {
        printf("✗ FAILED: Should have detected invalid side\n");
    }
    
    // TEST 6: Middle game position (should PASS)
    printf("\nTest 6: Valid middle game position\n");
    ParseFen("r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/3P1N2/PPP2PPP/RNBQK2R b KQkq - 0 4", &pos);
    UpdatePieceList(&pos);
    if(CheckBoard(&pos)) {
        printf("✓ PASSED: Middle game position is valid\n");
    } else {
        printf("✗ FAILED: Middle game position should be valid\n");
    }
    
    printf("\nAll tests completed!\n");
   
    
    
    cout<<endl<<"*************end*************"<<endl<<endl<<endl;
}
