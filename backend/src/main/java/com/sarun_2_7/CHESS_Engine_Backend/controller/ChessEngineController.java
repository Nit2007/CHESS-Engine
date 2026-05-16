package com.sarun_2_7.CHESS_Engine_Backend.controller;

import com.sarun_2_7.CHESS_Engine_Backend.service.ChessEngineService;
import com.sarun_2_7.CHESS_Engine_Backend.service.EngineException;
import com.sarun_2_7.CHESS_Engine_Backend.service.EngineStatus;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;

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
            BestMoveResponse res = new BestMoveResponse();
            res.bestMove = best;
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

    // --- simple request/response DTOs ---
    public static class PositionRequest { public String fen; }

    public static class BestMoveRequest { public String fen; public int depth; public long timeMs; }

    public static class BestMoveResponse { public String bestMove; public String error;
        public BestMoveResponse() {}
        public BestMoveResponse(String error) { this.error = error; }
    }

    public static class StartRequest { public int depth; }
}
