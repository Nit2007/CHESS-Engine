package com.sarun_2_7.CHESS_Engine_Backend.controller;

import com.sarun_2_7.CHESS_Engine_Backend.service.ChessEngineService;
import com.sarun_2_7.CHESS_Engine_Backend.service.EngineException;
import com.sarun_2_7.CHESS_Engine_Backend.service.EngineStatus;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;

@CrossOrigin(origins = "*")
@RestController
@RequestMapping("/api/engine")
public class ChessEngineController {

    private final ChessEngineService chessEngineService;

    public ChessEngineController(ChessEngineService chessEngineService) {
        this.chessEngineService = chessEngineService;
    }

    @PostMapping("/init")
    public ResponseEntity<Void> initEngine() {
        chessEngineService.startEngine();
        return ResponseEntity.ok().build();
    }

    @PostMapping("/position")
    public ResponseEntity<Void> setPosition(@RequestBody PositionRequest req) {
        if (req == null || req.fen == null) return ResponseEntity.badRequest().build();
        chessEngineService.setPosition(req.fen);
        return ResponseEntity.ok().build();
    }

    @PostMapping("/bestmove")
    public ResponseEntity<BestMoveResponse> getBestMove(@RequestBody BestMoveRequest req) {
        if (req == null || req.fen == null) return ResponseEntity.badRequest().build();
        // set position first
        chessEngineService.setPosition(req.fen);
        try {
            String best = chessEngineService.getBestMoveSync(req.depth <= 0 ? 6 : req.depth, req.timeMs);
            System.out.println("[DEBUG] Raw best move from JNI: " + best);
            BestMoveResponse res = new BestMoveResponse();
            // Some JNI wrappers return a JSON string with telemetry; detect and parse it.
            if (best != null && best.trim().startsWith("{")) {
                // simple and lenient parsing without extra deps
                res.bestMove = extractJsonString(best, "bestMove");
                Long nodes = extractJsonLong(best, "nodesVisited");
                Integer d = extractJsonInt(best, "depth");
                Long nps = extractJsonLong(best, "nps");
                Integer eval = extractJsonInt(best, "evaluation");
                res.nodesVisited = nodes;
                res.depth = d;
                res.nps = nps;
                res.evaluation = eval;
            } else {
                res.bestMove = best;
            }
            return ResponseEntity.ok(res);
        } catch (EngineException e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(new BestMoveResponse(e.getMessage()));
        }
    }

    @PostMapping("/start")
    public ResponseEntity<Void> startAnalysis(@RequestBody StartRequest req) {
        chessEngineService.startAnalysisAsync(req == null ? 0 : req.depth);
        return ResponseEntity.accepted().build();
    }

    @PostMapping("/stop")
    public ResponseEntity<Void> stopAnalysis() {
        chessEngineService.stopSearch();
        return ResponseEntity.ok().build();
    }

    @GetMapping("/status")
    public ResponseEntity<EngineStatus> getStatus() {
        EngineStatus status = chessEngineService.getStatus();
        return ResponseEntity.ok(status);
    }

    @GetMapping("/legalmoves")
    public ResponseEntity<List<String>> legalMoves() {
        return ResponseEntity.ok(chessEngineService.getLegalMoves());
    }

    @PostMapping("/reset")
    public ResponseEntity<Void> reset() {
        chessEngineService.resetEngine();
        return ResponseEntity.ok().build();
    }

    @PostMapping("/evaluate")
    public ResponseEntity<Integer> getEvaluationJson(@RequestBody FenRequest request) {
        String fen = request != null ? request.getFen() : null;
        if (fen != null ) {
            chessEngineService.setPosition(fen);
        }
        return ResponseEntity.ok(chessEngineService.evaluatePositionService(fen));
    }

    @GetMapping("/stockfish-eval")
    public ResponseEntity<String> getStockfishEval(@RequestParam String fen) {
        try {
            org.springframework.web.client.RestTemplate restTemplate = new org.springframework.web.client.RestTemplate();
            String url = "https://stockfish.online/api/s/v2.php?fen=" + java.net.URLEncoder.encode(fen, "UTF-8") + "&depth=12";
            String response = restTemplate.getForObject(url, String.class);
            return ResponseEntity.ok(response);
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("{\"error\":\"failed\"}");
        }
    }

    // ------------------------------- simple request/response DTOs ---------------------------------------
    public static class FenRequest {
        private String fen;
        public String getFen() { return fen; }
        public void setFen(String fen) { this.fen = fen; }
    }

    public static class PositionRequest { public String fen; }

    public static class BestMoveRequest { public String fen; public int depth; public long timeMs; }

    public static class BestMoveResponse { public String bestMove; public String error;
        // optional telemetry (filled when JNI returns JSON)
        public Long nodesVisited;
        public Integer depth;
        public Long nps;
        public Integer evaluation;

        public BestMoveResponse() {}
        public BestMoveResponse(String error) { this.error = error; }
    }

    public static class StartRequest { public int depth; }

    // --- lightweight JSON helpers (no external libs) ---
    private static String extractJsonString(String json, String key) {
        String pattern = "\"" + key + "\":\"";
        int idx = json.indexOf(pattern);
        if (idx < 0) return null;
        int start = idx + pattern.length();
        int end = json.indexOf('"', start);
        if (end < 0) return null;
        return json.substring(start, end);
    }

    private static Long extractJsonLong(String json, String key) {
        String pattern = "\"" + key + "\":";
        int idx = json.indexOf(pattern);
        if (idx < 0) return null;
        int start = idx + pattern.length();
        int end = start;
        while (end < json.length() && (Character.isDigit(json.charAt(end)) || json.charAt(end)=='-')) end++;
        try { return Long.parseLong(json.substring(start, end)); } catch (Exception e) { return null; }
    }

    private static Integer extractJsonInt(String json, String key) {
        Long v = extractJsonLong(json, key);
        return v == null ? null : v.intValue();
    }
}
