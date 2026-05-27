import React, { useState, useRef, useEffect } from 'react';
import { Chessboard } from 'react-chessboard';
import { api } from './engine/api';
import { Chess } from 'chess.js';

const moveSound = new Audio('https://raw.githubusercontent.com/lichess-org/lila/master/public/sound/standard/Move.ogg');
const captureSound = new Audio('https://raw.githubusercontent.com/lichess-org/lila/master/public/sound/standard/Capture.ogg');
const notifySound = new Audio('https://raw.githubusercontent.com/lichess-org/lila/master/public/sound/standard/GenericNotify.ogg');

function playSound(moveObj) {
  if (!moveObj) return;
  if (moveObj.captured || (moveObj.flags && (moveObj.flags.includes('c') || moveObj.flags.includes('e')))) {
    captureSound.play().catch(() => {});
  } else {
    moveSound.play().catch(() => {});
  }
}

export default function ChessBoardComponent() {
  const gameRef = useRef(new Chess());
  const [fen, setFen] = useState(gameRef.current.fen());
  const [legalDestinations, setLegalDestinations] = useState({});
  const [isEngineThinking, setIsEngineThinking] = useState(false);
  const [stats, setStats] = useState(null);
  
  // Settings
  const [engineColor, setEngineColor] = useState('b'); // 'w', 'b', 'both', 'none'
  const [gameResult, setGameResult] = useState(null);
  const [whiteDepth, setWhiteDepth] = useState(6);
  const [blackDepth, setBlackDepth] = useState(6);
  const [evalComparison, setEvalComparison] = useState(null);

  useEffect(() => {
    api.initEngine().catch(console.error);
  }, []);

  useEffect(() => {
    const game = gameRef.current;
    
    // Check for game over
    if (game.isGameOver()) {
      let res = 'Draw';
      if (game.isCheckmate()) {
         res = game.turn() === 'w' ? 'Black wins by checkmate' : 'White wins by checkmate';
      } else if (game.isStalemate()) {
         res = 'Draw by stalemate';
      } else if (game.isThreefoldRepetition()) {
         res = 'Draw by repetition';
      } else if (game.isInsufficientMaterial()) {
         res = 'Draw by insufficient material';
      } else if (game.isDraw()) {
         res = 'Draw';
      }
      setGameResult(res);
      notifySound.play().catch(() => {});
      return;
    }
    
    setGameResult(null);

    // Auto-trigger engine if it's its turn
    const turn = game.turn();
    if ((engineColor === 'both' || engineColor === turn) && !isEngineThinking) {
      makeEngineMove(game.fen(), turn);
    }
  }, [fen, engineColor]);

  async function resetGame() {
    gameRef.current = new Chess();
    setFen(gameRef.current.fen());
    setStats(null);
    setEvalComparison(null);
    setGameResult(null);
    try {
      await api.resetEngine();
    } catch (e) {
      console.error(e);
    }
  }

  function isUserTurn() {
    const turn = gameRef.current.turn();
    return engineColor !== 'both' && engineColor !== turn;
  }

  function onPromotionCheck(sourceSquare, targetSquare) {
    const moves = gameRef.current.moves({ square: sourceSquare, verbose: true });
    return moves.some(m => m.to === targetSquare && m.promotion);
  }

  async function onPromotionPieceSelect(piece, promoteFromSquare, promoteToSquare) {
    if (isEngineThinking || !isUserTurn()) return false;
    
    const promotionPiece = piece[1].toLowerCase();
    try {
      const move = gameRef.current.move({
        from: promoteFromSquare,
        to: promoteToSquare,
        promotion: promotionPiece
      });
      if (move) {
        playSound(move);
        setFen(gameRef.current.fen());
        return true;
      }
    } catch (e) {
      return false;
    }
    return false;
  }

  async function onDrop(sourceSquare, targetSquare) {
    if (isEngineThinking || !isUserTurn()) return false;
    
    const moves = gameRef.current.moves({ square: sourceSquare, verbose: true });
    const isPromotion = moves.some(m => m.to === targetSquare && m.promotion);
    if (isPromotion) {
      return false;
    }

    try {
      const move = gameRef.current.move({
        from: sourceSquare,
        to: targetSquare
      });
      if (move) {
        playSound(move);
        setFen(gameRef.current.fen());
        return true;
      }
    } catch (e) {
      return false;
    }
    return false;
  }

  async function makeEngineMove(currentFen, currentTurn) {
    setIsEngineThinking(true);
    const depth = currentTurn === 'w' ? whiteDepth : blackDepth;
    const startTime = performance.now();
    try {
      const response = await api.getBestMove(currentFen, depth);
      const endTime = performance.now();
      
      if (response && response.bestMove) {
        const from = response.bestMove.substring(0, 2);
        const to = response.bestMove.substring(2, 4);
        const promotion = response.bestMove.length > 4 ? response.bestMove.substring(4, 5) : undefined;
        
        try {
          const move = gameRef.current.move({ from, to, promotion });
          if (move) {
            playSound(move);
            setFen(gameRef.current.fen());
            
            const isBook = response.nodesVisited === 0;
            setStats({
               nodes: response.nodesVisited,
               depth: response.depth,
               nps: response.nps,
               eval: response.evaluation,
               timeMs: Math.round(endTime - startTime),
               retrievalMove: response.bestMove,
               isBook: isBook
            });

            // Fetch Stockfish evaluation for comparison
            if (!isBook) {
               api.getStockfishEval(currentFen).then(sfResponse => {
                   if (sfResponse && sfResponse.success) {
                       // Engine score is usually from its own perspective. 
                       // Convert to absolute (positive = white advantage)
                       const absEngineEval = (currentTurn === 'w' ? response.evaluation : -response.evaluation) / 100;
                       
                       let accuracy = "N/A";
                       if (sfResponse.mate == null) {
                           const diff = Math.abs(absEngineEval - sfResponse.evaluation);
                           if (diff <= 0.6) accuracy = "Highly Accurate";
                           else if (diff <= 1.5) accuracy = "Accurate";
                           else accuracy = "Inaccurate";
                       }
                       
                       setEvalComparison({
                           engineEval: absEngineEval.toFixed(2),
                           stockfishEval: sfResponse.mate != null ? `Mate in ${sfResponse.mate}` : sfResponse.evaluation,
                           accuracy: sfResponse.mate != null ? "N/A (Mate)" : accuracy
                       });
                   }
               }).catch(e => console.error("Stockfish fetch failed", e));
            } else {
               setEvalComparison(null);
            }
          }
        } catch (e) {
          console.error("Engine provided illegal move:", response.bestMove);
        }
      }
    } catch (err) {
      console.error("Error fetching engine move:", err);
    } finally {
      setIsEngineThinking(false);
    }
  }

  function onDragStart(sourceSquare) {
    if (isEngineThinking || !isUserTurn()) return;
    const moves = gameRef.current.moves({ square: sourceSquare, verbose: true });
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
    <div className="layout-container" style={{ display: 'flex', gap: '30px', alignItems: 'flex-start' }}>
      <div className="board-section">
        <div className="board-wrapper">
          <Chessboard
            position={fen}
            onPieceDrop={onDrop}
            onPieceDragBegin={onDragStart}
            onPieceDragEnd={onDragEnd}
            onPromotionCheck={onPromotionCheck}
            onPromotionPieceSelect={onPromotionPieceSelect}
            customSquareStyles={legalDestinations}
            arePiecesDraggable={!isEngineThinking && isUserTurn()}
            boardWidth={480}
            boardOrientation={engineColor === 'w' ? 'black' : 'white'}
          />
        </div>
      </div>
      
      <div className="sidebar" style={{ minWidth: '280px', padding: '20px', background: '#222', borderRadius: '8px', color: '#fff' }}>
        <div className="controls" style={{ display: 'flex', flexDirection: 'column', gap: '15px' }}>
          <div>
            <label style={{ display: 'block', marginBottom: '5px' }}>Engine Plays As:</label>
            <select 
              value={engineColor} 
              onChange={(e) => setEngineColor(e.target.value)}
              style={{ width: '100%', padding: '8px', borderRadius: '4px' }}
            >
              <option value="b">Black</option>
              <option value="w">White</option>
              <option value="both">Both (Engine vs Engine)</option>
              <option value="none">None (Analysis Board)</option>
            </select>
          </div>
          
          <div style={{ display: 'flex', gap: '10px' }}>
            <div style={{ flex: 1 }}>
              <label style={{ display: 'block', marginBottom: '5px', fontSize: '0.9em' }}>White Depth:</label>
              <input 
                type="number" 
                value={whiteDepth} 
                onChange={(e) => setWhiteDepth(parseInt(e.target.value) || 1)}
                min="1" max="20"
                style={{ width: '100%', padding: '6px', borderRadius: '4px' }}
              />
            </div>
            <div style={{ flex: 1 }}>
              <label style={{ display: 'block', marginBottom: '5px', fontSize: '0.9em' }}>Black Depth:</label>
              <input 
                type="number" 
                value={blackDepth} 
                onChange={(e) => setBlackDepth(parseInt(e.target.value) || 1)}
                min="1" max="20"
                style={{ width: '100%', padding: '6px', borderRadius: '4px' }}
              />
            </div>
          </div>
          
          <button onClick={resetGame} style={{ padding: '10px', cursor: 'pointer', background: '#4CAF50', color: 'white', border: 'none', borderRadius: '4px' }}>
            Reset Game
          </button>
        </div>

        {gameResult && (
          <div className="game-result" style={{ marginTop: '20px', padding: '15px', background: '#e74c3c', borderRadius: '5px', textAlign: 'center', fontWeight: 'bold' }}>
            <h3>Game Over</h3>
            <p>{gameResult}</p>
          </div>
        )}

        {stats && (
          <div className="stats" style={{ marginTop: '20px', padding: '15px', background: '#333', borderRadius: '5px' }}>
            <h4 style={{ margin: '0 0 10px 0', borderBottom: '1px solid #555', paddingBottom: '5px' }}>Engine Stats</h4>
            <p style={{ margin: '5px 0', color: '#4CAF50', fontWeight: 'bold' }}>
              <strong>Move:</strong> {stats.retrievalMove} {stats.isBook && ' 📖 (Book)'}
            </p>
            <p style={{ margin: '5px 0', color: '#ff9800' }}><strong>Time:</strong> {stats.timeMs} ms</p>
            {!stats.isBook && (
              <>
                <p style={{ margin: '5px 0' }}><strong>Depth:</strong> {stats.depth}</p>
                <p style={{ margin: '5px 0' }}><strong>Nodes:</strong> {stats.nodes != null ? stats.nodes.toLocaleString() : 'N/A'}</p>
                <p style={{ margin: '5px 0' }}><strong>NPS:</strong> {stats.nps != null ? stats.nps.toLocaleString() : 'N/A'}</p>
              </>
            )}
            
            {evalComparison && !stats.isBook && (
              <div style={{ marginTop: '15px', paddingTop: '10px', borderTop: '1px dashed #666' }}>
                <p style={{ margin: '5px 0' }}><strong>My Engine Eval:</strong> {evalComparison.engineEval}</p>
                <p style={{ margin: '5px 0', color: '#03A9F4' }}><strong>Stockfish Eval:</strong> {evalComparison.stockfishEval}</p>
                <p style={{ margin: '5px 0', color: evalComparison.accuracy === 'Highly Accurate' ? '#4CAF50' : evalComparison.accuracy === 'Accurate' ? '#FFC107' : '#F44336' }}>
                  <strong>Accuracy:</strong> {evalComparison.accuracy}
                </p>
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
}
