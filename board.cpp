#include "defs.h"
#include "struct.h"



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



uint64_t GeneratePosKey(const s_board* pos) {
    int sq = 0;
    int piece = EMPTY;
    uint64_t finalKey = 0;

    for (sq = 0; sq < BOARD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if (piece == OFFBOARD || piece == EMPTY) continue;
        if (piece < WP || piece > BK) 
            printf("BAD PIECE: %d at square %d\n", piece, sq);
        ASSERT(piece >= WP && piece <= BK);
        finalKey ^= PieceKeys[piece][sq];
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

void ResetBoard(s_board* pos)
{
    for(int i=0;i<BOARD_SQ_NUM;i++)
    {
        pos->pieces[i]=OFFBOARD;
    }
    for(int rank = 0; rank < 8; rank++) {
        for(int file = 0; file < 8; file++) {
            pos->pieces[smalltobig(file, rank)] = EMPTY;
        }
    }

    for(int i=0;i<3;i++)
    {
         pos->pawns[i]=0ULL;
         pos->bigpce[i]=0;
         pos->majpce[i]=0;
         pos->minpce[i]=0;
         pos->material[i]=0;
    }
    for(int i=0;i<13;i++)
    {//piecelist[13][10]
        for(int j = 0; j < 10; j++) {
            pos->piecelist[i][j]=0;
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

    while (*fen == ' ') fen++;
    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen++;

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

    while (*fen == ' ') fen++;
    if (*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';
        pos->enpas = smalltobig(file, rank);
        fen += 2;      
    }
    else {
    // be explicit
    pos->enpas = NO_SQ;
    fen++;                    // advance past '-'
}
    UpdatePieceList(pos);


pos->poskey = GeneratePosKey(pos);

}

string pce=".PNBRQKpnbrqk";   //piece list
string r="12345678";        //rank
string f="abcdefgh";        //file
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
    cout<<"   ";
    for(int file = 0; file < 8; file++) 
    {
        char FILE=file+97;
        cout<<FILE<<' '; // or some sq120 index
    }

    cout<<endl;
    cout<<"side to play : "<<side[pos->side]<<endl;
    cout<<"enpas square : "<<pos->enpas<<endl;
    cout << "castle permission : "
         << ((pos->castleperm & WKCA) ? 'K' : '-')
         << ((pos->castleperm & WQCA) ? 'Q' : '-')
         << ((pos->castleperm & BKCA) ? 'k' : '-')
         << ((pos->castleperm & BQCA) ? 'q' : '-') << endl;
    cout<<"POSITION's Zobrist HashKey : "<<pos->poskey<<endl;
}
// at data.cpp
//int pieceBig[13]={false,false,true,true,true,true,true,false,true,true,true,true,true};//excluding pawns
//int pieceMaj[13]={false,false,false,false,true,true,true,false,false,false,true,true,true};// R , Q , K
//int pieceMin[13]={false,false,true,true,false,false,false,false,true,true,false,false,false};// N , B
//int pieceVal[13]={0,100,325,325,500,1000,50000,100,325,325,500,1000,50000};
//int pieceCol[13]={BOTH,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};

void UpdatePieceList(s_board*pos)
{
    int piece,colour;
    // RESET ALL COUNTERS FIRST
    for(int i = 0; i < 3; i++) {
        pos->bigpce[i] = 0;
        pos->majpce[i] = 0; 
        pos->minpce[i] = 0;
        pos->material[i] = 0;
    }
    for(int i = 0; i < 13; i++) {
        pos->piecenum[i] = 0;
    }
    for(int i=0;i<BOARD_SQ_NUM ;i++)
    {
        piece=pos->pieces[i];
        if(piece!=OFFBOARD && piece!=EMPTY)
        {
            colour=pieceCol[piece];
            if(pieceBig[piece]==TRUE)pos->bigpce[colour]++;
            if(pieceMaj[piece]==TRUE)pos->majpce[colour]++;
            if(pieceMin[piece]==TRUE)pos->minpce[colour]++;
            pos->material[colour]+=pieceVal[piece];

            pos->piecelist[piece][pos->piecenum[piece]]=i;
            pos->piecenum[piece]++;
            
            if(piece==WK)pos->king[WHITE]=i;
            if(piece==BK)pos->king[BLACK]=i;

            if(piece==WP)
            {
                setBit(pos->pawns[WHITE],SQ64(i));
                setBit(pos->pawns[BOTH],SQ64(i));
            }
            if(piece==BP)
            {
                setBit(pos->pawns[BLACK],SQ64(i));
                setBit(pos->pawns[BOTH],SQ64(i));
            }
        }
    }
}

int CheckBoard(const s_board *pos)
{ //t_temporary variable
    int t_bigpce[3]={0,0,0};//int bigpce[3];
    int t_majpce[3]={0,0,0};//int majpce[3];
    int t_minpce[3]={0,0,0};//int minpce[3];
    int t_material[3]={0,0,0};//int material[3];
    uint64_t t_pawns[3]={0ULL,0ULL,0ULL};//uint64_t pawns[3];
    int t_piecenum[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};;// int piecenum[13];
    int sq120;
     
    for(int t_piece=WP;t_piece<=BK;t_piece++)//int piecelist[13][10];
    {
        for(int t_pce_idx=0; t_pce_idx<pos->piecenum[t_piece]; t_pce_idx++)  
        {
            sq120=pos->piecelist[t_piece][t_pce_idx];
            ASSERT(pos->pieces[sq120]==t_piece);
        }
    }
    int t_piece,colour;
    for(int i=0;i<64 ;i++)
    {
        sq120=SQ120(i);
        t_piece=pos->pieces[sq120];
        if(t_piece != EMPTY) 
        { 
            colour=pieceCol[t_piece];
            t_piecenum[t_piece]++;
            if(pieceBig[t_piece]==TRUE)t_bigpce[colour]++;
            if(pieceMaj[t_piece]==TRUE)t_majpce[colour]++;
            if(pieceMin[t_piece]==TRUE)t_minpce[colour]++;
            t_material[colour]+=pieceVal[t_piece];

            if(t_piece==WP)
            {
                setBit(t_pawns[WHITE],i);
                setBit(t_pawns[BOTH],i);
            }
            if(t_piece==BP)
             {
                setBit(t_pawns[BLACK],i);
                setBit(t_pawns[BOTH],i);
            }
        }
    }
    for(int t_piece=WP;t_piece<=BK;t_piece++)//int piecelist[13][10];
     {
         ASSERT(pos->piecenum[t_piece] == t_piecenum[t_piece]);
     }
    // CHECK PAWN COUNTS
    ASSERT(countBitBoard(pos->pawns[WHITE]) == pos->piecenum[WP]);
    ASSERT(countBitBoard(pos->pawns[BLACK]) == pos->piecenum[BP]);
    ASSERT(countBitBoard(pos->pawns[BOTH]) == (pos->piecenum[WP] +  pos->piecenum[BP]) );
    // CHECK BITBOARD SQUARES
    while(t_pawns[WHITE])
    {
        int sq64 = popBitBoard(&t_pawns[WHITE]);
        ASSERT(pos->pieces[SQ120(sq64)]==WP);
    }
    while(t_pawns[BLACK])
    {
        int sq64 = popBitBoard(&t_pawns[BLACK]);
        ASSERT(pos->pieces[SQ120(sq64)]==BP);
    }
    while(t_pawns[BOTH])
    {
        int sq64 = popBitBoard(&t_pawns[BOTH]);
        ASSERT(  (pos->pieces[SQ120(sq64)]==WP)  || (pos->pieces[SQ120(sq64)]==BP)  );
    }

    ASSERT( t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK] );
     ASSERT( t_bigpce[WHITE] == pos->bigpce[WHITE] && t_bigpce[BLACK] == pos->bigpce[BLACK] );
     ASSERT( t_majpce[WHITE] == pos->majpce[WHITE] && t_majpce[BLACK] == pos->majpce[BLACK] );
     ASSERT( t_minpce[WHITE] == pos->minpce[WHITE] && t_minpce[BLACK] == pos->minpce[BLACK] );

    ASSERT (pos->side == WHITE || pos->side == BLACK ) ;
    ASSERT(  GeneratePosKey(pos) == pos->poskey);
    ASSERT(pos->enpas == NO_SQ || 
       (pos->side == WHITE && RanksBrd[pos->enpas] == RANK_6) ||
       (pos->side == BLACK && RanksBrd[pos->enpas] == RANK_3));
    ASSERT( pos->pieces[pos->king[WHITE] ] == WK);
    ASSERT( pos->pieces[pos->king[BLACK] ] == BK);
    return true;
}
