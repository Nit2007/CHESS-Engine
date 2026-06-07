package com.sarun_2_7.CHESS_Engine_Backend.service.jni;

public class ChessEngineJNI {
    
    static {
        NativeLibLoader.load("native_engine");
    }
    public native void initEngine();
    
    public native void cleanupEngine();
    
    public native void setPosition(String fen);
    
    public native String getBestMove(int depth, long timeMs);
    
    public native String getBestMoveWithTimeControl(long wTime, long bTime, 
                                                   long wInc, long bInc, int movesToGo);
    
    public native int evaluatePosition(String fen);
    
    public native String[] getLegalMoves();
    
    public native boolean isMoveLegal(String move);
    
    public native void startInfiniteAnalysis();
    
    public native void stopSearch();
    
    public native boolean isSearching();
    
    public native String getEngineName();
    
    public native String getEngineAuthor();
    
    public native void resetEngine();
    
    public native boolean isCheckmate();
    
    public native boolean isStalemate();
    
    public native String getSideToMove();
    
    public native String[] getPrincipalVariation(int maxDepth);
}
