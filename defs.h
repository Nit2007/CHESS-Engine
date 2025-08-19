#pragma once
#include <iostream>
using namespace std;
#include <cstdlib> //FOR EXIT()
#include <cstdint> //FOR uint64_t
#include "struct.h"

#define MAXGAMEMOVES 2048
#define BOARD_SQ_NUM 120
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_1 "6k1/5p2/6p1/8/7p/8/6PP/6K1 b - - 0 0"
#define FEN_2 "3k4/2n2B2/1KP5/2B2p2/5b1p/7P/8/8 b - - 0 0"
#define FEN_3 "rnbqkbnr/pppppppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1"
#define FEN_4 "8/8/8/8/8/8/8/Q7 w - - 0 1"
#define PAWNMOVES_FEN "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
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

//enum Piece {EMPTY, WP, WN, WB, WR, WQ, WK,BP, BN, BB, BR, BQ, BK};// Piece types
#define INVALID -1
#define OFFBOARD -2
enum Piece {EMPTY, WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK};

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
    NO_SQ};// Board squares (in 120-square representation)
enum Bool {FALSE, TRUE};
enum Castle{WKCA =1,WQCA=2,BKCA=4,BQCA=8};  


#define RAND_64 ((uint64_t)rand() | \
                ((uint64_t)rand() << 15) | \
                ((uint64_t)rand() << 30) | \
                ((uint64_t)rand() << 45) | \
                (((uint64_t)rand() & 0xF) << 60))
/*    GAME MOVES [STORE - LEFT_SHIFT , EXTRACT - RIGHT_SHIFT] 
0000 0000 0000 0000 0000 0111 1111  ~> FROM 0x7F
0000 0000 0000 0011 1111 1000 0000  ~> TO >> 7 , 0x7F
0000 0000 0011 1100 0000 0000 0000  ~> CAPTURED >> 14 , 0xF
0000 0000 0100 0000 0000 0000 0000  ~> ENPAS , 0x40000
0000 0000 1000 0000 0000 0000 0000  ~> PAWN START , 0x80000
0000 1111 0000 0000 0000 0000 0000  ~> PROMOTED PIECE >> 20 , 0xF
0001 0000 0000 0000 0000 0000 0000  ~> CASTLE 0x1000000
*/
#define FROMSQ(m) ((m) & (0x7F) )
#define TOSQ(m) ((m>>7) & (0x7F) )
#define CAPTURED(m) ((m>>14) & (0xF) )
#define PROMOTED(m) ((m>>20) & (0xF) )
//MoveFLAG _ _
#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000
#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

// bitboard.cpp
extern int sq64tosq120[64];        //8 * 8
extern int sq120tosq64[BOARD_SQ_NUM];// 10 * 12
extern void init120to64();
extern void print12064();
extern void PrintBitBoard(uint64_t bb);
extern int SQ64(int sq120);
extern int SQ120(int sq64);
extern int countBitBoard(uint64_t bb);
extern int popBitBoard(uint64_t *bb);
extern uint64_t setBitMask[64];
extern uint64_t clearBitMask[64];
extern void initBitMask();
extern void setBit(uint64_t &bb , int sq);
extern void clearBit(uint64_t &bb , int sq);
extern int smalltobig(int file,int rank);
extern void PrintBin(int move);
// board.cpp
extern int fileCharToIndex(char c);
extern int rankCharToIndex(char c) ;
extern int getSquareFromString(const string& sq);
extern uint64_t GeneratePosKey(const s_board* pos) ;
extern void ResetBoard( s_board* pos);
extern void Parse_FEN(char* fen,s_board*pos);
extern void PrintBoard(s_board *pos);
extern void UpdatePieceList(s_board*pos);
extern int CheckBoard(const s_board *pos);
//init.cpp
extern int FilesBrd[BOARD_SQ_NUM];
extern int RanksBrd[BOARD_SQ_NUM];
extern void InitHashKeys();
extern void initsquare120();
extern void  InitFilesRanksBrd();
extern void allinit();
extern int square120[8][8];
extern uint64_t PieceKeys[13][120];
extern uint64_t SideKey;
extern uint64_t CastleKeys[16];

//attack.cpp
extern const int KnDir[8];
extern const int RkDir[4];
extern const int BiDir[4];
extern const int KiDir[8];
#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])
extern int SqAttacked(const int sq, const int side, const s_board *pos);
extern void showSqAttackBySide(const int side, const s_board*pos);

//data.cpp
extern int pieceBig[13];//excluding pawns
extern int pieceMaj[13];// R , Q , K
extern int pieceMin[13];// N , B
extern int pieceVal[13];
extern int pieceCol[13];
extern int PiecePawn[13];        
extern int PieceKnight[13] ;  
extern int PieceKing[13];       
extern int PieceRookQueen[13] ;
extern int PieceBishopQueen[13];
extern int PieceSlides[13] ;    

//io.cpp
extern char *PrSq(const int sq);
extern char *PrMove(const int move);
extern void PrintMoveList(const s_movelist *list);

//movegen.cpp
extern void AddQuietMove(const s_board *pos,int move ,  s_movelist *list);
extern void AddCaptureMove(const s_board *pos,int move ,  s_movelist *list);
extern void AddEnpasMove(const s_board *pos,int move ,  s_movelist *list);
extern void AddWhitePawnCapMove(const s_board *pos , const int from , const int to ,const int cap ,   s_movelist *list);
extern void AddWhitePawnMove(const s_board *pos , const int from , const int to ,   s_movelist *list);
extern void GenerateAllMoves(const s_board *pos ,  s_movelist *list);

//validate.cpp
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);









