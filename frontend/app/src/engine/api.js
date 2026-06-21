import axios from 'axios';

const isLocal = window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1';
const defaultRenderUrl = 'https://chess-backend.onrender.com/api/engine';
const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || (isLocal ? 'http://localhost:8080/api/engine' : defaultRenderUrl);

export const api = {
    initEngine: async () => {
        await axios.post(`${API_BASE_URL}/init`);
    },
    getBestMove: async (fen, depth = 6, timeMs = 0) => {
        const response = await axios.post(`${API_BASE_URL}/bestmove`, { fen, depth, timeMs });
        return response.data;
    },
    getEvaluation: async (fen) => {
        const response = await axios.post(`${API_BASE_URL}/evaluate`, { fen });
        return parseInt(response.data, 10);
    },
    getStockfishEval: async (fen) => {
        const response = await axios.get(`${API_BASE_URL}/stockfish-eval?fen=${encodeURIComponent(fen)}`);
        return response.data;
    },
    resetEngine: async () => {
        await axios.post(`${API_BASE_URL}/reset`);
    },
    getPv: async (maxDepth = 10) => {
        const response = await axios.get(`${API_BASE_URL}/pv?maxDepth=${maxDepth}`);
        return response.data;
    }
};
