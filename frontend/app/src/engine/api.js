import axios from 'axios';

// Flag to force using the local backend regardless of auto‑detect
let forceLocal = false;
export const setForceLocal = (value) => {
  forceLocal = !!value;
  // reset cached URL so the next request re‑evaluates the desired source
  cachedBaseUrl = null;
};

const defaultRenderUrl = 'https://chess-engine-backend.onrender.com/api/engine';
const LOCAL_URL = 'http://localhost:8080/api/engine';
let cachedBaseUrl = null;

// Determine which base URL to use, preferring local if reachable or forced
const getBaseUrl = async () => {
  if (forceLocal) {
    return LOCAL_URL;
  }
  return defaultRenderUrl;
};

export const api = {
  initEngine: async () => {
    const baseUrl = await getBaseUrl();
    await axios.post(`${baseUrl}/init`);
  },
  getBestMove: async (fen, depth = 6, timeMs = 0) => {
    const baseUrl = await getBaseUrl();
    const response = await axios.post(`${baseUrl}/bestmove`, { fen, depth, timeMs });
    return response.data;
  },
  getEvaluation: async (fen) => {
    const baseUrl = await getBaseUrl();
    const response = await axios.post(`${baseUrl}/evaluate`, { fen });
    return parseInt(response.data, 10);
  },
  getStockfishEval: async (fen) => {
    const baseUrl = await getBaseUrl();
    const response = await axios.get(`${baseUrl}/stockfish-eval?fen=${encodeURIComponent(fen)}`);
    return response.data;
  },
  resetEngine: async () => {
    const baseUrl = await getBaseUrl();
    await axios.post(`${baseUrl}/reset`);
  },
  getPv: async (maxDepth = 10) => {
    const baseUrl = await getBaseUrl();
    const response = await axios.get(`${baseUrl}/pv?maxDepth=${maxDepth}`);
    return response.data;
  }
};
