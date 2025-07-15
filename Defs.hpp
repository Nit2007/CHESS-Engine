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


void allinit();
int smalltobig(int file,int rank);
void init120to64();
void print12064();
int SQ64(int sq120);
void PrintBitBoard(uint64_t bb);
int popBitBoard(uint64_t *bb);
int countBitBoard(uint64_t bb);
void initsquare120();
int fileCharToIndex(char c);
int rankCharToIndex(char c);
int getSquareFromString(const string& sq);
void setBit(uint64_t bb , int sq);
void clearBit(uint64_t bb , int sq);


