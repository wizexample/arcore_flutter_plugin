#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "foregroundEditor.hpp"
#include <android/log.h>
#include "../data.hpp"

#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)

ForegroundEditor fcpp = ForegroundEditor();

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extOpen(
        JNIEnv *env, jobject obj,
        jlong baseAddr, jlong maskAddr, jlong bgAddr = 0) {

    auto baseImage = (Mat4b *) baseAddr;
    auto mask = (Mat1b *) maskAddr;

    fcpp.open(*baseImage, *mask);
    if (bgAddr != 0) {
        auto bgImage = (Mat4b *) bgAddr;
        fcpp.setBackground(*bgImage);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extClose(
        JNIEnv *env, jobject obj) {

    fcpp.close();
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extOnViewResized(
        JNIEnv *env, jobject obj,
        jfloat width, jfloat height) {

    fcpp.onViewResized(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extGetDisplayImage(
        JNIEnv *env, jobject obj,
        jlong destAddr) {

    auto destMat = (Mat4b *) destAddr;
    fcpp.getDisplayImage(*destMat);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extCreateBlankImage(
        JNIEnv *env, jobject obj,
        jlong destAddr, jint width, jint height) {

    auto destMat = (Mat1b *) destAddr;

    *destMat = Mat::ones(width, height, CV_8UC1) | 0xFF;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extGetMaskImage(
        JNIEnv *env, jobject obj,
        jlong destAddr) {

    auto destMat = (Mat1b *) destAddr;
    fcpp.getMaskImage(*destMat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extUpdateMaskImage(
        JNIEnv *env, jobject obj,
        jlong srcAddr) {

    auto srcMat = (Mat1b *) srcAddr;
    fcpp.updateMaskImage(*srcMat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetTouchMode(
        JNIEnv *env, jobject obj,
        jint mode) {

    fcpp.setTouchMode(mode);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetDrawWidth(
        JNIEnv *env, jobject obj,
        jint width) {

    fcpp.setDrawWidth(width);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extDrawOffset(
        JNIEnv *env, jobject obj,
        jint offset) {

    fcpp.setDrawOffset(offset);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extDrawPointer(
        JNIEnv *env, jobject obj,
        jfloat x, jfloat y, jfloat width, jfloat height) {

    fcpp.drawPointer(x, y, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extDrawMask(
        JNIEnv *env, jobject obj,
        jfloat x, jfloat y, jfloat width, jfloat height, jint size, jboolean isErase,
        jboolean isStart) {

    fcpp.drawMask(x, y, width, height, size, isErase, isStart);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetPosition(
        JNIEnv *env, jobject obj,
        jfloat x, jfloat y) {

    fcpp.setPosition(x, y);
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetScale(
        JNIEnv *env, jobject obj,
        jfloat scale) {

    return fcpp.setScale(scale);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extChangeBg(
        JNIEnv *env, jobject obj,
        jint pattern) {

    return fcpp.changeBg(pattern);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetChromaKeyPixel(
        JNIEnv *env, jobject obj,
        jfloat x, jfloat y, jfloat width, jfloat height, jboolean isStart) {

    fcpp.setChromaKeyPixel(x, y, width, height, isStart);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetChromaKeyColor(
        JNIEnv *env, jobject obj,
        jfloat h, jfloat s, jfloat v) {

    return fcpp.setChromaKeyColor(h, s, v);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetChromaKeyRange(
        JNIEnv *env, jobject obj,
        jfloat h, jfloat s, jfloat v) {

    fcpp.setChromaKeyRange(h, s, v);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetFloodFillPixel(
        JNIEnv *env, jobject obj,
        jfloat x, jfloat y, jfloat width, jfloat height, jboolean isStart) {

    return fcpp.setFloodFillPixel(x, y, width, height, isStart);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetFloodFillRange(
        JNIEnv *env, jobject obj,
        jfloat r, jfloat g, jfloat b) {

    fcpp.setFloodFillRange(r, g, b);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extToneAdjust(
        JNIEnv *env, jobject obj,
        jfloat h, jfloat s, jfloat v, jfloat contrast) {

    fcpp.toneAdjust(h, s, v, contrast);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetBlurParams(
        JNIEnv *env, jobject obj,
        jint size, jint erode) {

    fcpp.setBlurParams(size, erode);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extHistoryCount(
        JNIEnv *env, jobject obj) {

    return fcpp.historyCount();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extHistoryFirst(
        JNIEnv *env, jobject obj) {

    fcpp.historyFirst();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extHistoryBack(
        JNIEnv *env, jobject obj) {

    fcpp.historyBack();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extHistoryForward(
        JNIEnv *env, jobject obj) {

    fcpp.historyForward();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extHistoryLast(
        JNIEnv *env, jobject obj) {

    fcpp.historyLast();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extCancelLastHistory(
        JNIEnv *env, jobject obj) {

    fcpp.cancelLastHistory();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extShowPickedColor(
        JNIEnv *env, jobject obj) {
    fcpp.showPickedColor();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extHidePickedColor(
        JNIEnv *env, jobject obj) {
    fcpp.hidePickedColor();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_ForegroundEditor_extSetShifts(
        JNIEnv *env, jobject obj,
        jfloat dLeft, jfloat dTop) {

    fcpp.setShifts(dLeft, dTop);
}