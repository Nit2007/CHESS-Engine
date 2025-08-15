#include "defs.h"
#include "struct.h"

int SqOnBoard(const int sq) {
	return FilesBrd[sq]==OFFBOARD ? 0 : 1;
}

int SideValid(const int side) {
	return (side==WHITE || side == BLACK) ? 1 : 0;
}

int FileRankValid(const int fr) {
	return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int PieceValidEmpty(const int pce) {
	return (pce >= EMPTY && pce <= BK) ? 1 : 0;
}

int PieceValid(const int pce) {
	return (pce >= WP && pce <= BK) ? 1 : 0;
}
