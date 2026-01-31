#include "defs.h"
#include "struct.h"
#include <cstdio>
#include <cassert>
#include <vector>
#include <cstdlib>

const int polyPieceType[13] ={
  -1 , 1 , 3 , 5 ,7 ,9 , 11 , 0 ,2 , 4 ,6 , 8 ,10 
};

s_poly_book_entry* InitPolyBook(uint64_t &NumEntries){
	// Allow explicit override with POLYBOOK environment variable
	const char* envPath = getenv("POLYBOOK");

	std::vector<std::string> candidates;
	if(envPath && envPath[0] != '\0') {
		candidates.push_back(std::string(envPath));
	}
	candidates.push_back("komodo.bin");
	candidates.push_back("./komodo.bin");
	candidates.push_back("openingBook/komodo.bin");
	candidates.push_back("src/openingBook/komodo.bin");
	candidates.push_back("../openingBook/komodo.bin");
	candidates.push_back("../src/openingBook/komodo.bin");

	FILE *pFILE = nullptr;
	const char *usedPath = nullptr;
	for (size_t i = 0; i < candidates.size(); ++i) {
		pFILE = fopen(candidates[i].c_str(), "rb");
		if (pFILE) { usedPath = candidates[i].c_str(); break; }
	}

	if (pFILE == NULL) {
		cout<<"PolyBook is NULL\n";
		NumEntries = 0;
		return nullptr;
	} else {
		if(usedPath) cerr << "PolyBook opened from: " << usedPath << "\n";
		fseek(pFILE,0,SEEK_END);//Move POinter to the last
		uint64_t position = ftell(pFILE);//See How much the pointer have been moved
		if(position < sizeof(s_poly_book_entry)){
			cout<<"No Entries Found\n";
			fclose(pFILE);
			NumEntries = 0;
			return nullptr;
		}
		NumEntries = position / sizeof(s_poly_book_entry);
		#ifndef DEBUG
			if(usedPath) cout<<"Entries Found in FILE {"<<usedPath<<"} :"<<NumEntries;
			else cout<<"Entries Found in FILE :"<<NumEntries;
		#endif

		s_poly_book_entry *entries = new s_poly_book_entry[NumEntries];

		rewind(pFILE);
		size_t read = fread(entries, sizeof(s_poly_book_entry), (size_t)NumEntries, pFILE);
		if(read != (size_t)NumEntries){
			cerr<<"Failed to read all polybook entries\n";
			delete [] entries;
			fclose(pFILE);
			NumEntries = 0;
			return nullptr;
		}
		// Convert from big-endian (polyglot) to host endianness if necessary
		// Detect little-endian at runtime
		union { uint16_t u16; uint8_t b[2]; } endian = {1};
		bool isLittle = endian.b[0] == 1;
		if(isLittle){
			for(uint64_t i=0;i<NumEntries;++i){
				uint64_t k = entries[i].key;
				// swap 64
				entries[i].key = ((k & 0x00000000000000FFULL) << 56) |
								 ((k & 0x000000000000FF00ULL) << 40) |
								 ((k & 0x0000000000FF0000ULL) << 24) |
								 ((k & 0x00000000FF000000ULL) << 8)  |
								 ((k & 0x000000FF00000000ULL) >> 8)  |
								 ((k & 0x0000FF0000000000ULL) >> 24) |
								 ((k & 0x00FF000000000000ULL) >> 40) |
								 ((k & 0xFF00000000000000ULL) >> 56);

				uint16_t m = entries[i].move;
				entries[i].move = (uint16_t)((m<<8) | (m>>8));

				uint16_t w = entries[i].weight;
				entries[i].weight = (uint16_t)((w<<8) | (w>>8));

				uint32_t L = entries[i].learn;
				entries[i].learn = (uint32_t)(((L & 0x000000FFU) << 24) |
											  ((L & 0x0000FF00U) << 8)  |
											  ((L & 0x00FF0000U) >> 8)  |
											  ((L & 0xFF000000U) >> 24));
			}
		}

		fclose(pFILE);
		return entries;
	}
}
void CleanPolyBook(s_poly_book_entry* entries){
	if(entries) delete [] entries;
}
// Convert a polyglot-format 16-bit move to engine `move` encoding.
// polyMove bits: [from:6][to:6][prom:4]
static int PolyMoveToEngineMove(s_board* board, uint16_t polyMove){
	int to64 = polyMove & 0x3F;
	int from64 = (polyMove >> 6) & 0x3F;
	int prom = (polyMove >> 12) & 0xF; // 0=no prom, 1=knight,2=bishop,3=rook,4=queen

	int from = sq64tosq120[from64]; // map to 120-based square
	int to = sq64tosq120[to64];
	if(!SqOnBoard(from) || !SqOnBoard(to)) return 0; // invalid

	int movedPce = board->pieces[from];
	if(movedPce == EMPTY) return 0; // no piece to move

	int captured = board->pieces[to]; // captured piece or EMPTY

	int flag = 0;
	// Pawn two-square push -> set pawn start flag
	if(PiecePawn[movedPce]){
		int fromRank = RanksBrd[from];
		int toRank = RanksBrd[to];
		if(abs(toRank - fromRank) == 2) flag |= MFLAGPS;
	}

	int promoted = EMPTY;
	if(prom){
		// Map polyglot promotion (1..4) to engine piece (color-aware)
		if(board->side == WHITE) promoted = prom + 1; // 1->WN(2), 4->WQ(5)
		else promoted = prom + 7; // 1->BN(8), 4->BQ(11)
		flag |= MFLAGPROM;
	}

	return MOVE(from, to, captured, promoted, flag);
}
// Returns 0 if no book move found, otherwise an encoded `move` compatible with engine MOVE()
// Lookup a book move for `board` from `entries`. 
int GetBookMove(s_board* board, s_poly_book_entry* entries, uint64_t NumEntries){
	if(entries == nullptr || NumEntries == 0) return 0;

	uint64_t key = polyKeyFromBoard(board);

	// first pass: collect candidates and total weight
	uint64_t totalWeight = 0;
	struct Candidate { int move; uint16_t weight; };
	std::vector<Candidate> cand;
	cand.reserve(8);

	for(uint64_t i = 0; i < NumEntries; ++i){
		if(entries[i].key == key){
			int mv = PolyMoveToEngineMove(board, entries[i].move);
			if(mv == 0) continue; // skip invalid conversions
			cand.push_back({mv, entries[i].weight});
			totalWeight += entries[i].weight;
		}
	}

	if(cand.empty()) return 0;

	// weighted random selection
	uint64_t r = (uint64_t)rand();
	if(totalWeight > 0) r = r % totalWeight; // random in [0,totalWeight)

	uint64_t acc = 0;
	for(const auto &c : cand){
		acc += c.weight;
		if(r < acc) return c.move;
	}

	return cand.back().move;// fallback
}
int HasPawnForCapture(const s_board *board) {
	int sqWithPawn = 0;
	int targetPce = (board->side == WHITE) ? WP : BP;
	if(board->enpas != NO_SQ) {
		if(board->side == WHITE) {
			sqWithPawn = board->enpas - 10;
		} else {
			sqWithPawn = board->enpas + 10;
		}
		
		if(board->pieces[sqWithPawn + 1] == targetPce) {
			return TRUE;
		} else if(board->pieces[sqWithPawn - 1] == targetPce) {
			return TRUE;
		} 
	}
	return FALSE;
}

uint64_t polyKeyFromBoard(s_board* board){
    uint64_t finalKey = 0;
    int offset = 0;

    for(int sq=0;sq<BOARD_SQ_NUM;sq++){
        int piece = board->pieces[sq];
        if (piece == OFFBOARD || piece == EMPTY) continue;
        if (piece < WP || piece > BK) 
            printf("BAD PIECE: %d at square %d\n", piece, sq);
        ASSERT(piece >= WP && piece <= BK);
        int polyPiece = polyPieceType[piece];
        int file = FilesBrd[sq];
        int rank = RanksBrd[sq];
        //offset_piece=64*kind_of_piece+8*row+file;
        offset = 64 * (polyPiece) + (8 * rank) + file ;
        finalKey ^= Random64[offset];
    }
    // castling
	offset = 768;
	
	if(board->castleperm & WKCA) finalKey ^= Random64[offset + 0];
	if(board->castleperm & WQCA) finalKey ^= Random64[offset + 1];
	if(board->castleperm & BKCA) finalKey ^= Random64[offset + 2];
	if(board->castleperm & BQCA) finalKey ^= Random64[offset + 3];
	
	// enpassant
	offset = 772;
	if(HasPawnForCapture(board) == TRUE) {
		int file = FilesBrd[board->enpas];
		finalKey ^= Random64[offset + file];
	}
	
	if(board->side == WHITE) {
		finalKey ^= Random64[780];
	}
    return finalKey;
}