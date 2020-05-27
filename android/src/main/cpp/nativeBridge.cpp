#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include "opencv/nativeChannel/nativeExec.hpp"

#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)

NativeExec ncpp = NativeExec();

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_util_NativeMethodChannels_extMakeThumbnail(
        JNIEnv *env, jobject obj,
        jlong src, jlong dest, jint width, jint height, jfloat offset) {
    auto *srcMat = (cv::Mat4b *) src;
    auto *destMat = (cv::Mat4b *) dest;
    ncpp.makeThumbnail(*srcMat, *destMat, width, height, offset);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_util_NativeMethodChannels_extMakeInitialMask(
        JNIEnv *env, jobject obj,
        jlong src, jlong dest) {
    auto *srcMat = (cv::Mat4b *) src;
    auto *destMat = (cv::Mat1b *) dest;
    ncpp.makeInitialMask(*srcMat, *destMat);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_util_NativeMethodChannels_extImageNormalization(
        JNIEnv *env, jobject obj,
        jlong src, jlong dest, jint degree, jint size) {
    auto *srcMat = (cv::Mat4b *) src;
    auto *destMat = (cv::Mat4b *) dest;

    ncpp.imageNormalization(*srcMat, *destMat, degree, size);
}
