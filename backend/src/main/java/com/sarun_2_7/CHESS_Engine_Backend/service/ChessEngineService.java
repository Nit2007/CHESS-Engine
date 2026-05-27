package com.sarun_2_7.CHESS_Engine_Backend.service;

import com.sarun_2_7.CHESS_Engine_Backend.service.jni.ChessEngineJNI;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantLock;

@Service
public class ChessEngineService {
    private static final Logger log = LoggerFactory.getLogger(ChessEngineService.class);

    private final ReentrantLock engineLock = new ReentrantLock();
    private final ExecutorService searchExecutor = Executors.newSingleThreadExecutor(r -> new Thread(r, "engine-search"));
    private final AtomicBoolean initialized = new AtomicBoolean(false);

    public void startEngine(){
        if (initialized.get()) return;
        engineLock.lock();
        try {
            if (!initialized.get()) {
                ChessEngineJNI init = new ChessEngineJNI();
                init.initEngine();
                initialized.set(true);
                log.info("Chess engine initialized");
            }
        } finally {
            engineLock.unlock();
        }
    }

    public void stopEngine() {
        engineLock.lock();
        try {
            if (initialized.get()) {
                ChessEngineJNI jni = new ChessEngineJNI();
                jni.stopSearch();
                jni.cleanupEngine();
                initialized.set(false);
                log.info("Chess engine cleaned up");
            }
        } finally {
            engineLock.unlock();
            searchExecutor.shutdownNow();
        }
    }

    public void setPosition(String fen) {
        engineLock.lock();
        try {
            ChessEngineJNI jni = new ChessEngineJNI();
            jni.setPosition(fen);
        } finally {
            engineLock.unlock();
        }
    }

    /**
     * Synchronous best-move request. This method offloads the blocking native
     * search call to a single-threaded executor so HTTP request threads are
     * not blocked. The call waits for completion and returns the UCI move string.
     */
    public String getBestMoveSync(int depth, long timeMs) throws EngineException {
        if (!initialized.get()) {
            startEngine();
        }

        Callable<String> task = () -> {
            engineLock.lock();
            try {
                ChessEngineJNI jni = new ChessEngineJNI();
                String best = jni.getBestMove(depth, timeMs);
                return (best != null) ? best : "";
            } finally {
                engineLock.unlock();
            }
        };

        Future<String> future = searchExecutor.submit(task);
        try {
            // Wait unbounded here because timeMs is honored by native side;
            // callers can enforce their own upper bounds if desired.
            return future.get();
        } catch (InterruptedException e) {
            future.cancel(true);
            Thread.currentThread().interrupt();
            throw new EngineException("Search interrupted", e);
        } catch (ExecutionException e) {
            throw new EngineException("Search failed: " + e.getCause(), e.getCause());
        }
    }

    public void startAnalysisAsync(int depth) {
        searchExecutor.submit(() -> {
            engineLock.lock();
            try {
                ChessEngineJNI jni = new ChessEngineJNI();
                // native will run in blocking mode until stopSearch is called
                jni.startInfiniteAnalysis();
            } finally {
                engineLock.unlock();
            }
        });
    }

    public void stopSearch() {
        engineLock.lock();
        try {
            ChessEngineJNI jni = new ChessEngineJNI();
            jni.stopSearch();
        } finally {
            engineLock.unlock();
        }
    }

    public boolean isSearching() {
        engineLock.lock();
        try {
            ChessEngineJNI jni = new ChessEngineJNI();
            return jni.isSearching();
        } finally {
            engineLock.unlock();
        }
    }

    public EngineStatus getStatus() {
        EngineStatus s = new EngineStatus();
        engineLock.lock();
        try {
            ChessEngineJNI jni = new ChessEngineJNI();
            s.searching = jni.isSearching();
            s.engineName = jni.getEngineName();
            s.engineAuthor = jni.getEngineAuthor();
        } finally {
            engineLock.unlock();
        }
        return s;
    }

    public List<String> getLegalMoves() {
        engineLock.lock();
        try {
            ChessEngineJNI jni = new ChessEngineJNI();
            String[] moves = jni.getLegalMoves();
            return moves == null ? List.of() : List.of(moves);
        } finally {
            engineLock.unlock();
        }
    }

    public void resetEngine() {
        engineLock.lock();
        try {
            ChessEngineJNI jni = new ChessEngineJNI();
            jni.resetEngine();
        } finally {
            engineLock.unlock();
        }
    }


    public String getEvaluationJson() {
        ChessEngineJNI jni = new ChessEngineJNI();
        return jni.getEvaluationJson();
    }
}
