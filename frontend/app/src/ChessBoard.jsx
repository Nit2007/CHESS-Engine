import React, { useState, useRef } from 'react';
import { Chessboard } from 'react-chessboard';
import { applyMove, legalMoves, newGameFen } from './engine';

export default function ChessBoardComponent() {
  const gameRef = useRef({ fen: 'k7/P7/8/8/8/8/8/K7 w - - 0 1' });
  const [fen, setFen] = useState(gameRef.current.fen);
  const [legalDestinations, setLegalDestinations] = useState({});

  function resetGame() {
    gameRef.current = { fen: newGameFen() };
    setFen(gameRef.current.fen);
  }

  function onPromotionCheck(sourceSquare, targetSquare) {
    const moves = legalMoves(gameRef.current.fen, sourceSquare);
    return moves.some(m => m.to === targetSquare && m.promotion);
  }

  function onPromotionPieceSelect(piece, promoteFromSquare, promoteToSquare) {
    const promotionPiece = piece[1].toLowerCase();
    const result = applyMove(gameRef.current.fen, promoteFromSquare, promoteToSquare, promotionPiece);
    if (!result.ok) {
      return false;
    }
    gameRef.current.fen = result.fen;
    setFen(result.fen);
    return true;
  }

  function onDrop(sourceSquare, targetSquare) {
    // If it's a promotion move, let react-chessboard's promotion handler take care of it
    const moves = legalMoves(gameRef.current.fen, sourceSquare);
    const isPromotion = moves.some(m => m.to === targetSquare && m.promotion);
    if (isPromotion) {
      return false;
    }

    const result = applyMove(gameRef.current.fen, sourceSquare, targetSquare);
    if (!result.ok) {
      console.debug('Move blocked:', result.reason, result.legalMoves || '');
      return false;
    }
    gameRef.current.fen = result.fen;
    setFen(result.fen);
    return true;
  }

  function onDragStart(sourceSquare) {
    const moves = legalMoves(gameRef.current.fen, sourceSquare);
    if (!moves || moves.length === 0) {
      setLegalDestinations({});
      return;
    }
    const dests = {};
    moves.forEach(m => (dests[m.to] = { background: 'radial-gradient(circle, #00ff0044 40%, transparent 40%)' }));
    setLegalDestinations(dests);
  }

  function onDragEnd() {
    setLegalDestinations({});
  }

  return (
    <div className="layout-container">
      <div className="board-section">
        <h2>My Chess Engine</h2>
        <div className="board-wrapper">
          <Chessboard
            position={fen}
            onPieceDrop={onDrop}
            onPieceDragBegin={onDragStart}
            onPieceDragEnd={onDragEnd}
            onPromotionCheck={onPromotionCheck}
            onPromotionPieceSelect={onPromotionPieceSelect}
            customSquareStyles={legalDestinations}
            arePiecesDraggable={true}
            boardWidth={480}
          />
        </div>
        <div className="controls">
          <button onClick={resetGame}>Reset Game</button>
        </div>
      </div>
    </div>
  );
}
