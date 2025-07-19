#pragma once
#include <iostream>
using namespace std;
#include <cstdlib> //FOR EXIT()
#include <cstdint> //FOR uint64_t

#define MAXGAMEMOVES 2048
#define BOARD_SQ_NUM 120
// You define this only in debugging
#define DEBUG       

#ifndef DEBUG       //CHECKS IF DEBUG IS DEFINED
// In release mode: do nothing
    #define ASSERT(n)            
#else
// In debug mode: do actual checking        // Print error info and exit
    #define ASSERT(n)                           \
        if (!(n)) {                             \
            cerr << #n << " - Failed\n";        \
            cerr << "On " << __DATE__ << endl;  \
            cerr << "At " << __TIME__ << endl;  \
            cerr << "In File " << __FILE__ << endl; \
            cerr << "At Line " << __LINE__ << endl; \
            exit(1);                            \
        }
#endif

enum Piece {EMPTY, WP, WN, WB, WR, WQ, WK,BP, BN, BB, BR, BQ, BK};// Piece types
enum File {FILE_A, FILE_B, FILE_C, FILE_D,FILE_E, FILE_F, FILE_G, FILE_H,FILE_NONE};// File (column) indices
enum Rank {RANK_1, RANK_2, RANK_3, RANK_4,RANK_5, RANK_6, RANK_7, RANK_8,RANK_NONE};// Rank (row) indices
enum Side {WHITE, BLACK, BOTH};// Side to move
enum Square {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8,
    NO_SQ, OFFBOARD};// Board squares (in 120-square representation)
enum Bool {FALSE, TRUE};

#define RAND_64 ((uint64_t)rand() | \
                ((uint64_t)rand() << 15) | \
                ((uint64_t)rand() << 30) | \
                ((uint64_t)rand() << 45) | \
                (((uint64_t)rand() & 0xF) << 60))
// bitboard.cpp
extern int sq64tosq120[64];        //8 * 8
extern int sq120tosq64[BOARD_SQ_NUM];// 10 * 12
extern void init120to64();
extern void void print12064();
extern void PrintBitBoard(uint64_t bb);
extern int SQ64(int sq120);
extern int countBitBoard(uint64_t bb);
extern int popBitBoard(uint64_t *bb);
extern uint64_t setBitMask[64];
extern uint64_t clearBitMask[64];
extern void initBitMask();
extern void setBit(uint64_t &bb , int sq);
extern void clearBit(uint64_t &bb , int sq);
extern int smalltobig(int file,int rank);
// board.cpp
extern void initsquare120();
extern int fileCharToIndex(char c);
extern int rankCharToIndex(char c) ;
extern int getSquareFromString(const string& sq);
extern void InitHashKeys();
extern uint64_t GeneratePosKey(const s_board* pos) ;
extern void allinit();

