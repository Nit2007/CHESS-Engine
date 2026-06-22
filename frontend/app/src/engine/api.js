import axios from 'axios';

const defaultRenderUrl = 'https://chess-engine-backend.onrender.com/api/engine';
const LOCAL_URL = 'http://localhost:8080/api/engine';
let cachedBaseUrl = null;
// Determine which base URL to use, preferring local if reachable
const getBaseUrl = async () => {
  if (cachedBaseUrl) return cachedBaseUrl;
  try {
    // lightweight endpoint to test local backend
    await axios.get(`${LOCAL_URL}/status`, { timeout: 500 });
    cachedBaseUrl = LOCAL_URL;
  } catch (e) {
    cachedBaseUrl = defaultRenderUrl;
  }
  return cachedBaseUrl;
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
