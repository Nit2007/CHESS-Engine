#include <jni.h>
#include <string>
#include <iostream>
#include "defs.h"
#include "struct.h"

extern s_board g_board;
extern s_searchinfo g_info;
extern s_poly_book_entry* g_book;
extern uint64_t g_book_entries;
extern std::string g_engineName;
extern std::string g_engineAuthor;

extern "C" {

JNIEXPORT void JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_initEngine(JNIEnv *env, jobject obj) {
    UCI_Init();
}

JNIEXPORT void JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_cleanupEngine(JNIEnv *env, jobject obj) {
    if (g_book) {
        CleanPolyBook(g_book);
        g_book = nullptr;
    }
    ClearHashTable(g_board.hashtable);
}

JNIEXPORT void JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_setPosition(JNIEnv *env, jobject obj, jstring fen) {
    const char* fenStr = env->GetStringUTFChars(fen, 0);
    char fenBuffer[256];
    strncpy(fenBuffer, fenStr, 255);
    fenBuffer[255] = '\0';
    Parse_FEN(fenBuffer, &g_board);
    env->ReleaseStringUTFChars(fen, fenStr);
}

JNIEXPORT jstring JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_getBestMove(JNIEnv *env, jobject obj, jint depth, jlong timeMs) {
    g_info.starttime = GetTimeMs();
    g_info.stopped = false;
    g_info.timeset = (timeMs > 0);
    g_info.infinite = false;
    g_info.depth = (depth > 0 && depth <= MAXDEPTH) ? depth : 6;
    g_info.movestogo = 30;
    
    if (timeMs > 0) {
        g_info.stoptime = g_info.starttime + timeMs;
    }
    
    SearchPosition(&g_board, &g_info);
    std::string bestMove = std::string(PrMove(g_info.bestmove));
    return env->NewStringUTF(bestMove.c_str());
}

JNIEXPORT jstring JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_getBestMoveWithTimeControl(JNIEnv *env, jobject obj, 
                                                                                                            jlong wTime, jlong bTime, 
                                                                                                            jlong wInc, jlong bInc, jint movesToGo) {
    g_info.starttime = GetTimeMs();
    g_info.stopped = false;
    g_info.timeset = true;
    g_info.infinite = false;
    g_info.depth = MAXDEPTH;
    g_info.movestogo = (movesToGo > 0) ? movesToGo : 30;
    
    const int remaining = (g_board.side == WHITE) ? wTime : bTime;
    const int inc = (g_board.side == WHITE) ? wInc : bInc;
    int timeForMove = (remaining / g_info.movestogo) + inc;
    timeForMove = (timeForMove * 8) / 10;
    if (timeForMove < 50) timeForMove = 50;
    
    g_info.stoptime = g_info.starttime + timeForMove;
    SearchPosition(&g_board, &g_info);
    std::string bestMove = std::string(PrMove(g_info.bestmove));
    return env->NewStringUTF(bestMove.c_str());
}

JNIEXPORT jint JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_evaluatePosition(JNIEnv *env, jobject obj, jint depth) {
    return EvalPosition(&g_board);
}

JNIEXPORT jobjectArray JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_getLegalMoves(JNIEnv *env, jobject obj) {
    s_movelist list;
    GenerateAllMoves(&g_board, &list);
    
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray moveArray = env->NewObjectArray(list.count, stringClass, nullptr);
    
    for (int i = 0; i < list.count; i++) {
        std::string moveStr = std::string(PrMove(list.moves[i].move));
        jstring move = env->NewStringUTF(moveStr.c_str());
        env->SetObjectArrayElement(moveArray, i, move);
        env->DeleteLocalRef(move);
    }
    
    return moveArray;
}

JNIEXPORT jboolean JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_isMoveLegal(JNIEnv *env, jobject obj, jstring move) {
    const char* moveStr = env->GetStringUTFChars(move, 0);
    char moveBuffer[6];
    strncpy(moveBuffer, moveStr, 5);
    moveBuffer[5] = '\0';
    
    int moveInt = ParseMove(moveBuffer, &g_board);
    bool isLegal = (moveInt != FALSE);
    
    env->ReleaseStringUTFChars(move, moveStr);
    return isLegal ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_startInfiniteAnalysis(JNIEnv *env, jobject obj) {
    g_info.starttime = GetTimeMs();
    g_info.stopped = false;
    g_info.timeset = false;
    g_info.infinite = true;
    g_info.depth = MAXDEPTH;
    SearchPosition(&g_board, &g_info);
}

JNIEXPORT void JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_stopSearch(JNIEnv *env, jobject obj) {
    g_info.stopped = true;
}

JNIEXPORT jboolean JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_isSearching(JNIEnv *env, jobject obj) {
    return (!g_info.stopped && !g_info.quit) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_getEngineName(JNIEnv *env, jobject obj) {
    return env->NewStringUTF(g_engineName.c_str());
}

JNIEXPORT jstring JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_getEngineAuthor(JNIEnv *env, jobject obj) {
    return env->NewStringUTF(g_engineAuthor.c_str());
}

JNIEXPORT void JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_resetEngine(JNIEnv *env, jobject obj) {
    ResetBoard(&g_board);
    Parse_FEN((char*)START_FEN, &g_board);
    ClearForSearch(&g_board, &g_info);
}

JNIEXPORT jboolean JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_isCheckmate(JNIEnv *env, jobject obj) {
    s_movelist list;
    GenerateAllMoves(&g_board, &list);
    
    bool kingInAttack = SqAttacked(g_board.piecelist[g_board.side == WHITE ? WK : BK][0], 
                                  g_board.side ^ 1, &g_board);
    return (kingInAttack && list.count == 0) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_isStalemate(JNIEnv *env, jobject obj) {
    s_movelist list;
    GenerateAllMoves(&g_board, &list);
    
    bool kingInAttack = SqAttacked(g_board.piecelist[g_board.side == WHITE ? WK : BK][0], 
                                  g_board.side ^ 1, &g_board);
    return (!kingInAttack && list.count == 0) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_com_sarun_12_17_CHESS_1Engine_1Backend_service_jni_ChessEngineJNI_getSideToMove(JNIEnv *env, jobject obj) {
    const char* side = (g_board.side == WHITE) ? "white" : "black";
    return env->NewStringUTF(side);
}

}
