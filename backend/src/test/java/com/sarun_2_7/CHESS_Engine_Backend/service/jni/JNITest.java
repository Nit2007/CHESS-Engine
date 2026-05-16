package com.sarun_2_7.CHESS_Engine_Backend.service.jni;

import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;



public class JNITest {

    @Test
    public void testJNI() {
        ChessEngineJNI jni = new ChessEngineJNI();
        
        System.out.println("Initializing Engine...");
        jni.initEngine();
        System.out.println("Engine initialized.");
        
        System.out.println("Engine Name: " + jni.getEngineName());
        System.out.println("Engine Author: " + jni.getEngineAuthor());
        
        // Resetting engine
        jni.resetEngine();
        
        // Let's ask for the best move in the starting position
        System.out.println("Side to move: " + jni.getSideToMove());
        
        String bestMove = jni.getBestMove(3, 1000); // depth 3, 1 second
        System.out.println("Best move found: " + bestMove);
        assertNotNull(bestMove);
        assertFalse(bestMove.isEmpty());
        
        System.out.println("Legal moves:");
        String[] moves = jni.getLegalMoves();
        for (String move : moves) {
            System.out.print(move + " ");
        }
        System.out.println();
        assertTrue(moves.length > 0);
        
        jni.cleanupEngine();
        System.out.println("Cleaned up engine.");
    }
}
