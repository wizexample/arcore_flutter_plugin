#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include "bgErase.hpp"

BgErase ccpp = BgErase();

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extInit(
        JNIEnv *env, jobject obj,
        jlong insAddr) {
    auto *srcMat = (cv::Mat4b *) insAddr;

    ccpp.init(srcMat);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extSetChromaKeyRange(
        JNIEnv *env, jobject obj,
        jdouble h, jdouble s, jdouble v) {
    ccpp.setChromaKeyRange(h, s, v);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extToneAdjust(
        JNIEnv *env, jobject obj,
        jdouble h, jdouble s, jdouble v, jdouble contrast) {
    ccpp.toneAdjust((float)h, (float)s, (float)v, (float)contrast);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extSetChromaKeyColor(
        JNIEnv *env, jobject obj,
        jdouble h, jdouble s, jdouble v) {
    ccpp.setChromaKeyColor(h, s, v);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extChangeBg(
        JNIEnv *env, jobject obj,
        jint pattern) {
    ccpp.changeBg(pattern);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extSetBlurParams(
        JNIEnv *env, jobject obj,
        jint blur, jint erode) {
    ccpp.setBlurParams(blur, erode);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extGetDisplay(
        JNIEnv *env, jobject obj,
        jlong insAddr, jlong outsAddr) {
    auto *srcMat = (cv::Mat4b *) insAddr;
    auto *destMat = (cv::Mat4b *) outsAddr;

    ccpp.getDisplayImage(srcMat, *destMat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extGetBaseImage(
        JNIEnv *env, jobject obj,
        jlong outsAddr) {
    auto destMat = (cv::Mat4b *) outsAddr;

    ccpp.getBaseImage(*destMat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extGetMaskImage(
        JNIEnv *env, jobject obj,
        jlong outsAddr) {
    auto *destMat = (cv::Mat1b *) outsAddr;

    ccpp.getMaskImage(*destMat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extGetUnifiedImage(
        JNIEnv *env, jobject obj,
        jlong outsAddr) {
    auto *destMat = (cv::Mat4b *) outsAddr;

    ccpp.getUnifiedImage(*destMat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extGetSmallThumbnail(
        JNIEnv *env, jobject obj,
        jlong outsAddr, jint width, jint height, jfloat offset) {
    auto *destMat = (cv::Mat4b *) outsAddr;

    ccpp.getSmallThumbnail(*destMat, width, height, offset);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extSetChromakeyPixel(
        JNIEnv *env, jobject obj,
        jfloat x, jfloat y, jfloat width, jfloat height) {

    ccpp.setChromaKeyPixel(x, y, width, height);
//    ccpp.setChromaKeyPixel(y, width - x, height, width);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extShowPickedColor(
        JNIEnv *env, jobject obj) {
    ccpp.showPickedColor();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extHidePickedColor(
        JNIEnv *env, jobject obj) {
    ccpp.hidePickedColor();
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_flutter_1app3_platform_1view_CameraView_extSetChromakeyPointer(
        JNIEnv *env, jobject obj,
        jint pointer) {
    return ccpp.setChromakeysPointer(pointer);
}