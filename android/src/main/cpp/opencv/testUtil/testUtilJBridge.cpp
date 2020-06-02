#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include "testUtil.hpp"

#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)

TestUtil testUtil = TestUtil();

extern "C" JNIEXPORT void JNICALL
Java_com_difrancescogianmarco_arcore_1flutter_1plugin_opencv_TestUtil_extTest(JNIEnv *env, jobject thiz,
        jlong src, jlong dest) {
    auto *srcMat = (Mat4b *) src;
    auto *destMat = (Mat4b *) dest;

    testUtil.test(*srcMat, *destMat);
}