#include "defs.h"
#include "struct.h"

// PsqtDeltaOpening / PsqtDeltaEndgame are defined in evaluate.cpp.
// They have proper external linkage and full access to the const PSQT tables.
// IsEndgame / EvalPosition are also from evaluate.cpp (prototyped in defs.h).

// ─────────────────────────────────────────────────────────────────────────────
// InitEval
// Seeds evalOpening / evalEndgame by iterating over all pieces and summing
// their PSQT contributions into BOTH buckets independently.
//
// WHY NOT use EvalPosition() here:
//   EvalPosition() returns a single blended score for the *current* phase.
//   Splitting that one number into two buckets is wrong — if the phase later
//   flips (e.g. a queen is captured), the unused bucket would be 0 instead of
//   the true PSQT sum, causing the incremental value to diverge from reality.
//
//   By computing each bucket from scratch we stay in perfect sync with the
//   delta functions (IncrementEvalAddPiece / ClearPiece / MovePiece), which
//   always update both buckets on every move.
// ─────────────────────────────────────────────────────────────────────────────
void InitEval(s_board* pos) {
    int opening = 0;
    int endgame = 0;

    // Iterate over every piece type and accumulate PSQT contributions.
    // PsqtDeltaOpening / PsqtDeltaEndgame already handle colour mirroring
    // internally (black pieces return negative values), so the running sum
    // is always from White's perspective (absolute score).
    for (int pce = WP; pce <= BK; ++pce) {
        for (int i = 0; i < pos->piecenum[pce]; ++i) {
            int sq  = pos->piecelist[pce][i];
            int idx = SQ64(sq);
            opening += PsqtDeltaOpening(pce, idx);
            endgame += PsqtDeltaEndgame(pce, idx);
        }
    }

    pos->evalOpening = opening;
    pos->evalEndgame = endgame;
}

// ─────────────────────────────────────────────────────────────────────────────
// IncrementEvalAddPiece
// Called by AddPiece (makemove.cpp) after a piece is placed on the board.
// Adds the PSQT contribution for the new square.
// ─────────────────────────────────────────────────────────────────────────────
void IncrementEvalAddPiece(const int sq, const int pce, s_board* pos) {
    int idx = SQ64(sq);
    pos->evalOpening += PsqtDeltaOpening(pce, idx);
    pos->evalEndgame += PsqtDeltaEndgame(pce, idx);
}

// ─────────────────────────────────────────────────────────────────────────────
// IncrementEvalClearPiece
// Called by ClearPiece (makemove.cpp) before a piece is removed from the board.
// Removes the PSQT contribution for the vacated square.
// ─────────────────────────────────────────────────────────────────────────────
void IncrementEvalClearPiece(const int sq, const int pce, s_board* pos) {
    int idx = SQ64(sq);
    pos->evalOpening -= PsqtDeltaOpening(pce, idx);
    pos->evalEndgame -= PsqtDeltaEndgame(pce, idx);
}

// ─────────────────────────────────────────────────────────────────────────────
// IncrementEvalMovePiece
// Called by MovePiece (makemove.cpp) when a piece slides from one square to
// another. Efficiently updates both tables in one pass.
// ─────────────────────────────────────────────────────────────────────────────
void IncrementEvalMovePiece(const int from, const int to, const int pce, s_board* pos) {
    int idxFrom = SQ64(from);
    int idxTo   = SQ64(to);
    pos->evalOpening -= PsqtDeltaOpening(pce, idxFrom);
    pos->evalOpening += PsqtDeltaOpening(pce, idxTo);
    pos->evalEndgame -= PsqtDeltaEndgame(pce, idxFrom);
    pos->evalEndgame += PsqtDeltaEndgame(pce, idxTo);
}

// ─────────────────────────────────────────────────────────────────────────────
// GetIncrementalEval
// Drop-in replacement for EvalPosition in the search hot-path.
// Returns score from the perspective of the side to move (same convention).
//
// When to use which:
//   • GetIncrementalEval(pos) — quiescence stand-pat, depth-limit fallback.
//     O(1), uses cached material + PSQT.
//   • EvalPosition(pos)       — full board evaluation, called only for
//     positions loaded fresh (InitEval), or as a correctness sanity check.
// ─────────────────────────────────────────────────────────────────────────────
int GetIncrementalEval(s_board* pos) {
    // Material difference (always live via ClearPiece / AddPiece)
    int mat  = pos->material[WHITE] - pos->material[BLACK];
    // PSQT: pick the bucket that matches the current game phase
    int psqt = IsEndgame(pos) ? pos->evalEndgame : pos->evalOpening;
    int score = mat + psqt;
    return (pos->side == WHITE) ? score : -score;
}
