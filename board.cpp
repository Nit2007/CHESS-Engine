#include "defs.h"
#include "struct.h"

int square120[8][8];
//FOR CONVERTING CHESS SQUARE NAME TO (FILE,RANK)
void initsquare120(){
for(int rank=0;rank<8;rank++)
    {
        for(int file=0;file<8;file++)
            {
                square120[rank][file]=smalltobig(file,rank);
            }
    }
}
int fileCharToIndex(char c) {
    return tolower(c) - 'a'; // 'a' = 0
}

int rankCharToIndex(char c) {
    return c - '1'; // '1' = 0
}

int getSquareFromString(const string& sq) {
    if (sq.length() != 2) return -1;
    int file = fileCharToIndex(sq[0]);
    int rank = rankCharToIndex(sq[1]);
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return -1;
    return square120[rank][file];
}

uint64_t PieceKeys[13][120];
uint64_t SideKey;
uint64_t CastleKeys[16];

void InitHashKeys() {
   for (int index = 0; index < 13; ++index) {
        for (int index2 = 0; index2 < 120; ++index2) {
            PieceKeys[index][index2] = RAND_64;
        }
    }
SideKey = RAND_64;
    for (int index = 0; index < 16; ++index) {
        CastleKeys[index] = RAND_64;
    }
}

uint64_t GeneratePosKey(const s_board* pos) {
    int sq = 0;
    int piece = EMPTY;
    uint64_t finalKey = 0;

    for (sq = 0; sq < BOARD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if (piece != NO_SQ && piece != EMPTY) {
            ASSERT(piece >= WP && piece <= BK);
            finalKey ^= PieceKeys[piece][sq];
        }
    }

    if (pos->side == WHITE) {
        finalKey ^= SideKey;
    }

    if (pos->enpas != NO_SQ) {
        ASSERT(pos->enpas >= 0 && pos->enpas < BOARD_SQ_NUM);
        finalKey ^= PieceKeys[EMPTY][pos->enpas];
    }

    ASSERT(pos->castleperm >= 0 && pos->castleperm <= 15);
    finalKey ^= CastleKeys[pos->castleperm];

    return finalKey;
}
void ResetBoard( s_board* pos)
{
    for(int i=0;i<BOARD_SQ_NUM;i++)
    {
        pos->pieces[i]=OFFBOARD;
    }
    for(int i=0;i<64;i++)
    { //  8*8 is setted as EMPTY ,LEAVING REMAINING OFFBOARD
        pos->pieces[smalltobig(i)]=EMPTY;
    }
    for(int i=0;i<3;i++)
    {
         pos->pawns[i]=0ULL;
         pos->bigpce[i]=0;
         pos->majpce[i]=0;
         pos->minpce[i]=0;
    }
    for(int i=0;i<13;i++)
    {//piecelist[13][10]
        for(int j = 0; j < 10; j++) {
            piecelist[i][j]=0;
        }
    }
    pos->king[WHITE]=pos->king[BLACK]= NO_SQ ; //int king[3];

     pos->side=BOTH;    //int side;
     pos->enpas=NO_SQ;   //int enpas;
     pos->fifty=0;  //int fifty;

     pos->ply=0;          //NUMBER OF HALF MOVES PLAYED IN THE CURRENT SEARCH ( resets to 0 in a new search )
     pos->hisply=0;       // total NUMBER OF half HISTORY MOVES PLAYED IN THE WHOLE GAME
     pos->castleperm=0;
    
     pos->poskey=0ULL;        //uint64_t poskey;
}
void Parse_FEN(char* fen,s_board*pos)
{
    ASSERT(fen!=NULL);
    ASSERT(pos!=NULL);
    int piece =0; int count=1;int sq120=0;    int rank=RANK_8;    int file=FILE_A;
    ResetBoard(pos);
    while(rank>=RANK_1 && *fen)
    {
      count=1;
       if (*fen >= '1' && *fen <= '8') 
       {
           count = *fen - '0';
           piece = EMPTY;
        } else {
            switch (*fen) {
                case 'p': piece = BP; break;
                case 'r': piece = BR; break;
                case 'n': piece = BN; break;
                case 'b': piece = BB; break;
                case 'k': piece = BK; break;
                case 'q': piece = BQ; break;
                case 'P': piece = WP; break;
                case 'R': piece = WR; break;
                case 'N': piece = WN; break;
                case 'B': piece = WB; break;
                case 'K': piece = WK; break;
                case 'Q': piece = WQ; break;
                case '/':
                case ' ':
                    rank--;
                    file = FILE_A;
                    fen++;
                    continue;
                default:
                    cerr << "FEN error at: " << *fen << endl;
                    return;
            }
        }

        for (int j = 0; j < count; j++) {
            if (piece != EMPTY) {
                sq120 = smalltobig(file, rank);
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    // ========== Side to Move ========== //
    while (*fen == ' ') fen++;
    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen++;

    // ========== Castling Rights ========== //
    while (*fen == ' ') fen++;
    while (*fen != ' ') {
        switch (*fen) {
            case 'K': pos->castleperm |= WKCA; break;
            case 'Q': pos->castleperm |= WQCA; break;
            case 'k': pos->castleperm |= BKCA; break;
            case 'q': pos->castleperm |= BQCA; break;
            case '-': break;
        }
        fen++;
    }
    
      // ========== En Passant Square ========== //
    while (*fen == ' ') fen++;
    if (*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';
        pos->enpas = smalltobig(file, rank);
    }
    pos->poskey = GeneratePosKey(pos);
}
string pce=".PNBRQKpnbrqk";   //piece list
string r="12345678";        //rank
stirng f="abcdefgh";        //file
string side="wb-";
void PrintBoard(s_board *pos)
{
    int sq,file,rank,piece;
    for(rank=RANK_8;rank>=RANK_1;rank--)
    {
        cout<<rank+1<<" ";
        for(file=FILE_A;file<=FILE_H;file++)
        {
            sq=smalltobig(file,rank);
            piece=pos->pieces[sq];
            cout<<" "<<pce[piece];
        }
        cout<<endl;
    }
    for(file=FILE_A;file<=FILE_H;file++)
    {
        cout<<'a'+file;
    }
    cout<<endl;
    cout<<"side to play : "<<side[pos->side]<<endl;
    cout<<"enpas square : "<<pos->enpas<<endl;
    cout<<"castle permission : "<<(pos->castleperm & WKCA)?'K':'-'<<(pos->castleperm & WQCA)?'Q':'-'
        <<(pos->castleperm & BKCA)?'k':'-'<<(pos->castleperm & BQCA)?'q':'-'<<endl;
    cout<<"POSITION's Zobrist HashKey : "<<pos->poskey<<endl;
}
void allinit()
{
    init120to64();
    initsquare120();
    InitHashKeys();
}
