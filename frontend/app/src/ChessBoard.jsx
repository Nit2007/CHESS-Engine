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
    captureSound.play().catch(() => { });
  } else {
    moveSound.play().catch(() => { });
  }
}

export default function ChessBoardComponent() {
  const gameRef = useRef(new Chess());
  const [fen, setFen] = useState(gameRef.current.fen());
  const [legalDestinations, setLegalDestinations] = useState({});
  const [isEngineThinking, setIsEngineThinking] = useState(false);
  const [stats, setStats] = useState(null);
  const pvGameRef = useRef(new Chess());
  const [pvFen, setPvFen] = useState(pvGameRef.current.fen());
  const animationIdRef = useRef(0);
  const graphCanvasRef = useRef(null);
  const [lastMove, setLastMove] = useState(null);

  // Connection state for huge green message
  const [isConnected, setIsConnected] = useState(false);
  const [showDeploySuccess, setShowDeploySuccess] = useState(false);

  // Settings
  const [engineColor, setEngineColor] = useState('b'); // 'w', 'b', 'both', 'none'
  const [gameResult, setGameResult] = useState(null);
  const [whiteDepth, setWhiteDepth] = useState(6);
  const [blackDepth, setBlackDepth] = useState(6);
  const [moveNumber, setMoveNumber] = useState(0);
  const [npsData, setNpsData] = useState([]);
  const [evalComparison, setEvalComparison] = useState(null);

  useEffect(() => {
    const checkConnection = async () => {
      try {
        await api.initEngine();
        setIsConnected(prev => {
          if (!prev) {
            // Just connected!
            setShowDeploySuccess(true);
          }
          return true;
        });
      } catch (e) {
        setIsConnected(false);
      }
    };
    checkConnection();
    const intervalId = setInterval(checkConnection, 3000);
    return () => clearInterval(intervalId);
  }, []);

  useEffect(() => {
    const handleKeyDown = () => setShowDeploySuccess(false);
    if (showDeploySuccess) {
      window.addEventListener('keydown', handleKeyDown);
    }
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [showDeploySuccess]);

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
      notifySound.play().catch(() => { });
      return;
    }

    setGameResult(null);

    // Auto-trigger engine if it's its turn
    const turn = game.turn();
    if ((engineColor === 'both' || engineColor === turn) && !isEngineThinking) {
      makeEngineMove(game.fen(), turn);
    }
  }, [fen, engineColor, isEngineThinking]);

  async function resetGame() {
    gameRef.current = new Chess();
    setFen(gameRef.current.fen());
    setStats(null);
    setEvalComparison(null);
    setLastMove(null);
    setLegalDestinations({});
    setGameResult(null);
    setMoveNumber(0);
    setNpsData([]);
    pvGameRef.current = new Chess();
    setPvFen(pvGameRef.current.fen());
    animationIdRef.current++;
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

  function formatEngineMove(game, moveStr) {
    let from = moveStr.substring(0, 2);
    let to = moveStr.substring(2, 4);
    const promotion = moveStr.length > 4 ? moveStr.substring(4, 5) : undefined;

    try {
      const piece = game.get(from);
      if (piece && piece.type === 'k') {
        if (from === 'e1' && to === 'a1') to = 'c1';
        if (from === 'e1' && to === 'h1') to = 'g1';
        if (from === 'e8' && to === 'a8') to = 'c8';
        if (from === 'e8' && to === 'h8') to = 'g8';
      }
    } catch (e) { }

    return { from, to, promotion };
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
        setLastMove({ from: move.from, to: move.to });
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
        setLastMove({ from: move.from, to: move.to });
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

      try {
        const pvData = await api.getPv(10);
        console.log("PV Data received from backend:", pvData);
        if (pvData && pvData.length > 0) {
          console.log("Starting PV Animation with FEN:", currentFen);
          animatePv(currentFen, pvData);
        } else {
          console.warn("PV Data was empty!");
        }
      } catch (e) {
        console.error("Failed to fetch PV! Is the backend rebuilt and restarted?", e);
      }

      if (response && response.bestMove) {
        let { from, to, promotion } = formatEngineMove(gameRef.current, response.bestMove);
        let finalBestMove = response.bestMove;

        // Prevent premature draws
        try {
          const testGame = new Chess(gameRef.current.fen());
          testGame.move({ from, to, promotion });
          if (testGame.isThreefoldRepetition()) {
            const engineEval = (currentTurn === 'w' ? response.evaluation : -response.evaluation) / 100;
            if (Math.abs(engineEval) > 0.5) {
              console.warn("Premature draw detected! Engine Eval is", engineEval, "> 0.5. Forcing alternative move.");
              const legalMoves = gameRef.current.moves({ verbose: true });
              let bestAltMove = null;
              let bestAltScore = currentTurn === 'w' ? -Infinity : Infinity;

              for (const m of legalMoves) {
                const moveNotation = m.from + m.to + (m.promotion || '');
                if (moveNotation === finalBestMove || moveNotation === response.bestMove) continue;

                const tempGame = new Chess(gameRef.current.fen());
                tempGame.move(m);

                try {
                  const score = await api.getEvaluation(tempGame.fen());
                  if (currentTurn === 'w') {
                    if (score > bestAltScore) { bestAltScore = score; bestAltMove = m; }
                  } else {
                    if (score < bestAltScore) { bestAltScore = score; bestAltMove = m; }
                  }
                } catch (e) { }
              }

              if (bestAltMove) {
                finalBestMove = bestAltMove.from + bestAltMove.to + (bestAltMove.promotion || '');
                from = bestAltMove.from;
                to = bestAltMove.to;
                promotion = bestAltMove.promotion;
                console.log("Selected alternative move:", finalBestMove, "with static score:", bestAltScore);
              }
            }
          }
        } catch (e) { }

        try {
          const move = gameRef.current.move({ from, to, promotion });
          if (move) {
            playSound(move);
            setLastMove({ from: move.from, to: move.to });
            setFen(gameRef.current.fen());

            const isBook = response.nodesVisited === 0;
            const engineEvalNum = (currentTurn === 'w' ? response.evaluation : -response.evaluation) / 100;
            // Update stats and track move number & NPS
            setStats(prev => ({
              ...prev,
              nodes: response.nodesVisited,
              depth: response.depth,
              nps: response.nps,
              eval: engineEvalNum,
              timeMs: Math.round(endTime - startTime),
              retrievalMove: response.bestMove,
              isBook: isBook
            }));
            // Increment move number and record NPS data (skip zero NPS / book moves)
            setMoveNumber(prev => {
              const newMove = prev + 1;
              if (response.nps > 0) {
                setNpsData(prevData => [...prevData, { move: newMove, nps: response.nps }]);
              }
              return newMove;
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

  async function animatePv(startFen, moves) {
    const currentId = ++animationIdRef.current;
    pvGameRef.current.load(startFen);
    setPvFen(startFen);

    await new Promise(r => setTimeout(r, 400));

    for (let i = 0; i < moves.length; i++) {
      if (animationIdRef.current !== currentId) break;
      const moveStr = moves[i];
      if (!moveStr || moveStr === "0000") break;

      const { from, to, promotion } = formatEngineMove(pvGameRef.current, moveStr);

      try {
        const moveRes = pvGameRef.current.move({ from, to, promotion });
        if (!moveRes) break;
        setPvFen(pvGameRef.current.fen());
        await new Promise(r => setTimeout(r, 800));
      } catch (e) { break; }
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
    <>
      {showDeploySuccess && (
        <div
          onClick={() => setShowDeploySuccess(false)}
          style={{
            position: 'fixed', top: 0, left: 0, right: 0, bottom: 0,
            zIndex: 9998, background: 'rgba(0,0,0,0.6)',
            display: 'flex', justifyContent: 'center', alignItems: 'center'
          }}>
          <div
            onClick={(e) => e.stopPropagation()}
            style={{
              position: 'relative',
              background: 'linear-gradient(135deg, #4CAF50, #2E7D32)', color: 'white',
              padding: '40px 60px', borderRadius: '15px', zIndex: 9999, textAlign: 'center',
              boxShadow: '0 20px 50px rgba(0,0,0,0.8)', border: '5px solid #81C784',
              animation: 'popIn 0.5s cubic-bezier(0.175, 0.885, 0.32, 1.275)'
            }}>
            <button onClick={() => setShowDeploySuccess(false)} style={{ position: 'absolute', top: '10px', right: '15px', background: 'transparent', border: 'none', color: 'white', fontSize: '24px', cursor: 'pointer', fontWeight: 'bold' }}>&times;</button>
            <h1 style={{ fontSize: '3em', margin: '0 0 15px 0', textShadow: '2px 2px 4px rgba(0,0,0,0.5)' }}>🎉 DEPLOYMENT COMPLETED! 🎉</h1>
            <h2 style={{ fontSize: '2em', margin: 0, fontWeight: 'normal' }}>You can play with <strong style={{ color: '#FFEB3B' }}>Sarun_2_7</strong> now! ♟️</h2>
            <p style={{ marginTop: '20px', fontSize: '16px', opacity: 0.8 }}>(Press any key or click anywhere to dismiss)</p>
          </div>
        </div>
      )}
      <style>{`
        @keyframes popIn {
          0% { opacity: 0; transform: scale(0.8); }
          100% { opacity: 1; transform: scale(1); }
        }
        .eval-bar {
          width: 25px;
          height: 100%;
          background: #333;
          border-radius: 4px;
          overflow: hidden;
          position: relative;
          display: flex;
          flex-direction: column;
          justify-content: flex-end;
          box-shadow: inset 0 0 5px rgba(0,0,0,0.8);
        }
        .eval-fill {
          background: #f0f0f0;
          width: 100%;
          transition: height 0.5s ease-in-out;
        }
        .eval-text {
          position: absolute;
          top: 50%;
          left: 50%;
          transform: translate(-50%, -50%);
          font-size: 10px;
          font-weight: bold;
          color: #ff3333;
          background: rgba(0,0,0,0.6);
          padding: 2px 4px;
          border-radius: 3px;
          white-space: nowrap;
          z-index: 10;
        }
        .layout-container {
          display: flex;
          gap: 20px;
          align-items: flex-start;
        }
        @media (max-width: 768px) {
          .layout-container {
            flex-direction: column;
            align-items: stretch;
          }
          .layout-container > .sidebar,
          .layout-container > .board-section,
          .layout-container > .sidebar.pv-board {
            min-width: 100% !important;
            padding: 10px;
          }
          .layout-container > .board-section {
            display: flex;
            justify-content: center;
          }
        }
      `}</style>

      <div className="layout-container" style={{ display: 'flex', gap: '20px', alignItems: 'flex-start' }}>
        <div className="sidebar pv-board" style={{ minWidth: '220px', padding: '15px', background: '#222', borderRadius: '8px' }}>
          <h3 style={{ margin: '0 0 10px 0', fontSize: '16px', color: '#aaa', textAlign: 'center' }}>Thought Process</h3>
          <Chessboard
            position={pvFen}
            boardWidth={220}
            arePiecesDraggable={false}
            showNotation={false}
            boardOrientation={engineColor === 'w' ? 'black' : 'white'}
          />
          {/* NPS Graph */}
          <div style={{ marginTop: '10px', background: '#111', borderRadius: '8px', padding: '6px', border: '1px solid #2a2a2a', width: '110%' }}>
            <div style={{ fontSize: '15px', color: '#4CAF50', fontWeight: 'bold', textAlign: 'center', marginBottom: '4px' }}>
              ⚡Nodes Per Second
            </div>
            {(() => {
              const W = 800, H = 600, pad = { top: 12, right: 12, bottom: 30, left: 40 };
              const plotW = W - pad.left - pad.right;
              const plotH = H - pad.top - pad.bottom;
              const rawMax = npsData.length > 0 ? Math.max(...npsData.map(d => d.nps)) : 1;
              const maxNps = Math.ceil(rawMax * 1.2); // add 20% headroom
              const yTicks = [0, 0.5, 1].map(f => Math.round(maxNps * f));
              const formatNps = (v) => v >= 1000000 ? (v / 1000000).toFixed(1) + 'M' : v >= 1000 ? (v / 1000).toFixed(0) + 'K' : String(v);
              const pts = npsData.map((d, i) => ({
                x: pad.left + (npsData.length > 1 ? (i / (npsData.length - 1)) * plotW : plotW / 2),
                y: pad.top + plotH - (d.nps / maxNps) * plotH
              }));
              return (
                <svg width="100%" viewBox={`0 0 ${W} ${H}`} preserveAspectRatio="xMidYMid meet" style={{ display: 'block' }}>
                  <defs>
                    <linearGradient id="npsAreaGrad" x1="0" y1="0" x2="0" y2="1">
                      <stop offset="0%" stopColor="#4CAF50" stopOpacity="0.45" />
                      <stop offset="100%" stopColor="#4CAF50" stopOpacity="0.02" />
                    </linearGradient>
                  </defs>
                  <rect x={pad.left} y={pad.top} width={plotW} height={plotH} fill="#080808" rx="2" />
                  {/* Grid + Y labels */}
                  {yTicks.map((tick, i) => {
                    const y = pad.top + plotH - (tick / maxNps) * plotH;
                    return (
                      <g key={i}>
                        <line x1={pad.left} y1={y} x2={pad.left + plotW} y2={y} stroke="#1a1a1a" strokeWidth="0.5" strokeDasharray="3,3" />
                        <text x={pad.left - 2} y={y + 3} fill="#555" fontSize="7" textAnchor="end" fontFamily="monospace">{formatNps(tick)}</text>
                      </g>
                    );
                  })}
                  {/* X labels */}
                  {npsData.length > 0 && npsData.map((d, i) => {
                    if (npsData.length <= 12 || i % Math.ceil(npsData.length / 8) === 0 || i === npsData.length - 1) {
                      return <text key={i} x={pts[i].x} y={H - 3} fill="#555" fontSize="7" textAnchor="middle" fontFamily="monospace">{d.move}</text>;
                    }
                    return null;
                  })}
                  {/* Axes */}
                  <line x1={pad.left} y1={pad.top} x2={pad.left} y2={pad.top + plotH} stroke="#333" strokeWidth="1" />
                  <line x1={pad.left} y1={pad.top + plotH} x2={pad.left + plotW} y2={pad.top + plotH} stroke="#333" strokeWidth="1" />
                  {/* Area + Line */}
                  {pts.length > 1 && (
                    <>
                      <path d={`M${pts[0].x},${pts[0].y} ${pts.slice(1).map(p => `L${p.x},${p.y}`).join(' ')} L${pts[pts.length - 1].x},${pad.top + plotH} L${pts[0].x},${pad.top + plotH} Z`} fill="url(#npsAreaGrad)" />
                      <polyline fill="none" stroke="#4CAF20" strokeWidth="2" points={pts.map(p => `${p.x},${p.y}`).join(' ')} strokeLinejoin="round" strokeLinecap="round" />
                    </>
                  )}
                  {/* Points */}
                  {pts.map((p, i) => (
                    <circle key={i} cx={p.x} cy={p.y} r="3" fill="#4CAF50" stroke="#111" strokeWidth="1">
                      <animate attributeName="r" from="0" to="3" dur="0.3s" fill="freeze" />
                    </circle>
                  ))}
                  {/* Empty placeholder */}
                  {/* {npsData.length === 0 && (
                    <text x={W / 2} y={H / 2} fill="#444" fontSize="100" textAnchor="middle">Book moves played, waiting for search</text>
                  )} */}
                  {npsData.length === 0 && (
                      <text
                        x={W / 2}
                        y={H / 2}
                        fill="#444"
                        fontSize="80"
                        textAnchor="middle"
                        fontFamily="monospace"
                      >
                        <tspan x={W / 2} dy="-100">Book moves </tspan>
                        <tspan x={W / 2} dy="100">played ,Waiting </tspan>
                        <tspan x={W / 2} dy="100">for search...</tspan>
                      </text>
                    )}
                </svg>
              );
            })()}
            {/* Stats footer */}
            <div style={{ display: 'flex', justifyContent: 'space-around', alignItems: 'center', marginTop: '8px', padding: '6px 2px', background: '#0a0a0a', borderRadius: '5px' }}>
              <div style={{ textAlign: 'center' }}>
                <div style={{ fontSize: '8px', color: '#555', textTransform: 'uppercase', letterSpacing: '0.5px' }}>Moves</div>
                <div style={{ fontSize: '15px', color: '#ccc', fontWeight: 'bold', fontFamily: 'monospace' }}>{moveNumber}</div>
              </div>
              <div style={{ width: '1px', height: '26px', background: '#2a2a2a' }} />
              <div style={{ textAlign: 'center' }}>
                <div style={{ fontSize: '8px', color: '#555', textTransform: 'uppercase', letterSpacing: '0.5px' }}>Avg NPS</div>
                <div style={{ fontSize: '15px', color: '#4CAF50', fontWeight: 'bold', fontFamily: 'monospace' }}>{(npsData.reduce((s, d) => s + d.nps, 0) / (npsData.length || 1) / 1000000).toFixed(2)}M</div>
              </div>
              <div style={{ width: '1px', height: '26px', background: '#2a2a2a' }} />
              <div style={{ textAlign: 'center' }}>
                <div style={{ fontSize: '8px', color: '#555', textTransform: 'uppercase', letterSpacing: '0.5px' }}>Peak</div>
                <div style={{ fontSize: '15px', color: '#ff9800', fontWeight: 'bold', fontFamily: 'monospace' }}>{(npsData.length > 0 ? Math.max(...npsData.map(d => d.nps)) / 1000000 : 0).toFixed(2)}M</div>
              </div>
            </div>
          </div>
        </div>

        <div style={{ display: 'flex', flexDirection: 'column', gap: '5px', padding: '10px 0', alignItems: 'center' }}>
          <div style={{ fontSize: '10px', color: '#aaa', fontWeight: 'bold' }}>EVAL</div>
          <div style={{ display: 'flex', gap: '8px', height: '480px' }}>
            <div className="eval-bar" title="Engine Evaluation">
              <div className="eval-text">{stats && stats.eval !== undefined ? (stats.eval > 0 ? '+' : '') + stats.eval.toFixed(1) : '0.0'}</div>
              <div className="eval-fill" style={{ height: `${Math.max(0, Math.min(100, 50 + ((stats?.eval || 0) * 10)))}%` }} />
              <div style={{ position: 'absolute', top: '50%', width: '100%', height: '2px', background: 'red', zIndex: 5 }} />
            </div>
            <div className="eval-bar" title="Stockfish (Cloud) Evaluation">
              <div className="eval-text">{evalComparison && evalComparison.stockfishEval !== undefined ? (typeof evalComparison.stockfishEval === 'string' ? evalComparison.stockfishEval.replace('Mate in ', 'M') : (evalComparison.stockfishEval > 0 ? '+' : '') + evalComparison.stockfishEval.toFixed(1)) : '-'}</div>
              <div className="eval-fill" style={{ height: `${Math.max(0, Math.min(100, 50 + ((evalComparison && typeof evalComparison.stockfishEval === 'number' ? evalComparison.stockfishEval : (evalComparison?.stockfishEval ? 10 : 0)) * 10)))}%`, background: '#88aaff' }} />
              <div style={{ position: 'absolute', top: '50%', width: '100%', height: '2px', background: 'red', zIndex: 5 }} />
            </div>
          </div>
          <div style={{ display: 'flex', width: '100%', justifyContent: 'space-between', fontSize: '9px', color: '#777' }}>
            <span>ENG</span>
            <span>SF</span>
          </div>
        </div>

        <div className="board-section" style={{ display: 'flex', alignItems: 'center' }}>
          <div className="board-wrapper">
            <Chessboard
              position={fen}
              onPieceDrop={onDrop}
              onPieceDragBegin={onDragStart}
              onPieceDragEnd={onDragEnd}
              onPromotionCheck={onPromotionCheck}
              onPromotionPieceSelect={onPromotionPieceSelect}
              customSquareStyles={{
                ...legalDestinations,
                ...(lastMove ? {
                  [lastMove.from]: { background: 'rgba(144, 238, 144, 0.5)' },
                  [lastMove.to]: { background: 'rgba(144, 238, 144, 0.5)' }
                } : {})
              }}
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

            <button
              onClick={() => {
                const deployCmd = '@echo off\ncolor 0A\necho =========================================================================\necho             ALMOST THERE! STARTING BACKEND ENGINE...\necho =========================================================================\necho  *** Wait for the "Started ChessEngineBackendApplication" message ***\necho  ***   Once you see it, you can PLAY WITH SARUN_2_7 in your app!  ***\necho =========================================================================\n' + String.fromCharCode(27) + '[94m\necho *************************************************************************\necho ***                                                                   ***\necho ***                       MINIMIZE THE TERMINAL                       ***\necho ***                                                                   ***\necho *************************************************************************\n' + String.fromCharCode(27) + '[92m\nif not exist "CHESS-Engine" (\n  git clone https://github.com/Nit2007/CHESS-Engine.git\n)\ncd CHESS-Engine/backend\ncall mvnw.cmd clean spring-boot:run\npause';
                const blob = new Blob([deployCmd], { type: 'text/plain' });
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = 'run_chess_engine.bat';
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
                URL.revokeObjectURL(url);

                navigator.clipboard.writeText('git clone https://github.com/Nit2007/CHESS-Engine.git || true && cd CHESS-Engine/backend && ./mvnw clean spring-boot:run');
                alert("Downloaded 'run_chess_engine.bat'!\n\nJust double-click the downloaded file to auto-open your terminal and run the engine. (The command is also copied to your clipboard as a backup!)");
              }}
              style={{ padding: '10px', cursor: 'pointer', background: 'linear-gradient(45deg, #FFD700, #F39C12)', color: '#000', border: '1px solid #E67E22', borderRadius: '4px', fontWeight: 'bold', boxShadow: '0 4px 15px rgba(243, 156, 18, 0.4)' }}
            >
              ⚡ Self Deploy <span style={{ fontSize: '11px', fontWeight: 'normal' }}>(For better performance)</span>
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
            </div>
          )}
        </div>

        {/* Catchy Left Side Bottom Links */}
        <div className="social-links-left" style={{ position: 'fixed', bottom: '30px', left: '30px', display: 'flex', flexDirection: 'column', gap: '15px', zIndex: 1000 }}>
          <a href="https://github.com/Nit2007/CHESS-Engine" target="_blank" rel="noopener noreferrer" style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', width: '45px', height: '45px', borderRadius: '50%', background: '#2c2c2c', color: '#fff', transition: 'all 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275)', boxShadow: '0 4px 10px rgba(0,0,0,0.5)', textDecoration: 'none' }} onMouseOver={e => { e.currentTarget.style.transform = 'translateY(-5px) scale(1.1)'; e.currentTarget.style.boxShadow = '0 8px 15px rgba(0,0,0,0.6)'; e.currentTarget.style.background = '#333'; }} onMouseOut={e => { e.currentTarget.style.transform = 'none'; e.currentTarget.style.boxShadow = '0 4px 10px rgba(0,0,0,0.5)'; e.currentTarget.style.background = '#2c2c2c'; }}>
            <svg viewBox="0 0 24 24" style={{ width: '22px', height: '22px', fill: 'currentColor' }}><path d="M12 0C5.37 0 0 5.37 0 12c0 5.31 3.435 9.795 8.205 11.385.6.105.825-.255.825-.57 0-.285-.015-1.23-.015-2.235-3.015.555-3.795-.735-4.035-1.41-.135-.345-.72-1.41-1.23-1.695-.42-.225-1.02-.78-.015-.795.945-.015 1.62.87 1.845 1.23 1.08 1.815 2.805 1.305 3.495.99.105-.78.42-1.305.765-1.605-2.67-.3-5.46-1.335-5.46-5.925 0-1.305.465-2.385 1.23-3.225-.12-.3-.54-1.53.12-3.18 0 0 1.005-.315 3.3 1.23.96-.27 1.98-.405 3-.405s2.04.135 3 .405c2.295-1.56 3.3-1.23 3.3-1.23.66 1.65.24 2.88.12 3.18.765.84 1.23 1.905 1.23 3.225 0 4.605-2.805 5.625-5.475 5.925.435.375.81 1.095.81 2.22 0 1.605-.015 2.895-.015 3.3 0 .315.225.69.825.57A12.02 12.02 0 0024 12c0-6.63-5.37-12-12-12z" /></svg>
          </a>
          <a href="https://www.linkedin.com/in/nithish-jaisarun-a-7256b932b/" target="_blank" rel="noopener noreferrer" style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', width: '45px', height: '45px', borderRadius: '50%', background: '#2c2c2c', color: '#fff', transition: 'all 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275)', boxShadow: '0 4px 10px rgba(0,0,0,0.5)', textDecoration: 'none' }} onMouseOver={e => { e.currentTarget.style.transform = 'translateY(-5px) scale(1.1)'; e.currentTarget.style.boxShadow = '0 8px 15px rgba(0,0,0,0.6)'; e.currentTarget.style.background = '#0077b5'; }} onMouseOut={e => { e.currentTarget.style.transform = 'none'; e.currentTarget.style.boxShadow = '0 4px 10px rgba(0,0,0,0.5)'; e.currentTarget.style.background = '#2c2c2c'; }}>
            <svg viewBox="0 0 24 24" style={{ width: '22px', height: '22px', fill: 'currentColor' }}><path d="M20.447 20.452h-3.554v-5.569c0-1.328-.027-3.037-1.852-3.037-1.853 0-2.136 1.445-2.136 2.939v5.667H9.351V9h3.414v1.561h.046c.477-.9 1.637-1.85 3.37-1.85 3.601 0 4.267 2.37 4.267 5.455v6.286zM5.337 7.433c-1.144 0-2.063-.926-2.063-2.065 0-1.138.92-2.063 2.063-2.063 1.14 0 2.064.925 2.064 2.063 0 1.139-.925 2.065-2.064 2.065zm1.782 13.019H3.555V9h3.564v11.452zM22.225 0H1.771C.792 0 0 .774 0 1.729v20.542C0 23.227.792 24 1.771 24h20.451C23.2 24 24 23.227 24 22.271V1.729C24 .774 23.2 0 22.222 0h.003z" /></svg>
          </a>
          <a href="https://lichess.org/@/Sarun_2_7" target="_blank" rel="noopener noreferrer" style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', width: '45px', height: '45px', borderRadius: '50%', background: '#2c2c2c', color: '#fff', transition: 'all 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275)', boxShadow: '0 4px 10px rgba(0,0,0,0.5)', textDecoration: 'none' }} onMouseOver={e => { e.currentTarget.style.transform = 'translateY(-5px) scale(1.1)'; e.currentTarget.style.boxShadow = '0 8px 15px rgba(0,0,0,0.6)'; e.currentTarget.style.background = '#444'; e.currentTarget.style.color = '#fff'; }} onMouseOut={e => { e.currentTarget.style.transform = 'none'; e.currentTarget.style.boxShadow = '0 4px 10px rgba(0,0,0,0.5)'; e.currentTarget.style.background = '#2c2c2c'; e.currentTarget.style.color = '#fff'; }}>
            <img src="https://cdn.simpleicons.org/lichess/white" alt="Lichess" style={{ width: '22px', height: '22px', display: 'block' }} />
          </a>
        </div>
      </div>
    </>
  );
}
