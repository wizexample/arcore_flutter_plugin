#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include "bgErase.hpp"

#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)


///////////////////////////////////////////////////////////////////////////////////////////////////
//// JNI APIs
///////////////////////////////////////////////////////////////////////////////////////////////////
int width, height = 0;
int x, y = -1;
//BgReduce2 bgReduce = BgReduce2();
BgErase2 bgErase = BgErase2();

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_execChroma(
        JNIEnv *env,
        jobject obj,
        jlong src,
        jlong dest) {
    auto *srcMat = (Mat4b *) src;
    auto *destMat = (Mat *) dest;

    if (width <= 0 && height <= 0) {
        // 最初のフレームで初期化
        Size size = srcMat->size();
        width = size.width;
        height = size.height;

        bgErase.init(srcMat);
    }
    if (width <= 0 && height <= 0) {
        // 初期化失敗
        srcMat->copyTo(*destMat);
        return;
    }

    try {
        bgErase.exec(srcMat, *destMat);
    } catch (std::exception e) {
        log2("exception: %s", e.what());
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid = env->GetMethodID(cls, "reset", "()V");
        env->CallVoidMethod(obj, mid);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_setBGColor(
        JNIEnv *env,
        jobject,
        jint r,
        jint g,
        jint b) {
//    bgImage.setTo(Scalar(255, 0, 0));
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_setCompeteImg(
        JNIEnv *env,
        jobject,
        jlong matAddr) {
    auto *mat = (Mat4b *) matAddr;
    bgErase.setCompeteImg(*mat);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_setTouchPos(
        JNIEnv *env,
        jobject,
        jfloat touchedX,
        jfloat touchedY) {

    x = static_cast<int>(touchedX * width);
    y = static_cast<int>(touchedY * height);
    __android_log_print(ANDROID_LOG_DEBUG, "Tag", "Touch Position %d, %d", x, y);

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_setThreshold(
        JNIEnv *env,
        jobject,
        jint threshold
) {
    bgErase.setThreshold(threshold);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_testErodeParams(
        JNIEnv *env,
        jobject,
        jint erodeConnection,
        jint erodeIter
) {
    bgErase.setTestErodeParams(erodeConnection, erodeIter);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_testBlurParams(
        JNIEnv *env,
        jobject,
        jint type,
        jint size1, jint size2,
        jint p1, jint p2, jint p3
) {
    log2("type:%d, size: (%d, %d), p1: %d, p2: %d, p3: %d", type, size1, size2, p1, p2, p3);
    bgErase.setTestBlurParams(type, size1, size2, p1, p2, p3);
//    callJava();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_SecondaryActivity_testSubtractMethod(
        JNIEnv *env,
        jobject,
        jint method
) {
    bgErase.setTestSubtractMethod(method);
}
