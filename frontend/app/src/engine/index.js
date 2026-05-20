import { Chess } from 'chess.js';

// applyMove: applies a move given a FEN, from, to, optional promotion.
// Returns { ok: true, fen, move } on success or { ok: false, reason, legalMoves }
export function applyMove(fen, from, to, promotion = 'q') {
  const game = new Chess(fen);
  if (game.isGameOver()) return { ok: false, reason: 'game_over' };

  const piece = game.get(from);
  if (!piece) return { ok: false, reason: 'no_piece' };
  if (piece.color !== game.turn()) return { ok: false, reason: 'wrong_side' };

  const move = game.move({ from, to, promotion });
  if (move === null) {
    return { ok: false, reason: 'illegal', legalMoves: game.moves({ verbose: true }) };
  }

  return { ok: true, fen: game.fen(), move };
}

export function legalMoves(fen, square) {
  const game = new Chess(fen);
  return game.moves({ square, verbose: true });
}

export function newGameFen() {
  return new Chess().fen();
}
