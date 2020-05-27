#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include "layoutEditor.hpp"
#include "../data.hpp"

#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)

LayoutEditor lcpp = LayoutEditor();

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extGetDisplayImage(
        JNIEnv *env, jobject obj,
        jlong destAddr) {

    auto dest = (Mat4b *) destAddr;
    lcpp.getDisplayImage(*dest);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extCreateProject(
        JNIEnv *env, jobject obj,
        jlong id, jstring name, jlong bgId, jlong imgId,
        jboolean uped, jstring thumb, jlong fId) {

    CProject p = CProject();
    p.id = id;
    p.name = env->GetStringUTFChars(name, 0);
    p.bgId = bgId;
    p.mainImageId = imgId;
    p.uploaded = uped;
    p.thumbnail = env->GetStringUTFChars(thumb, 0);
    p.folderId = fId;

    lcpp.initProject(p);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extAddForeground(
        JNIEnv *env, jobject obj,
        jlong id, jint zIndex, jfloat xAxis, jfloat yAxis,
        jfloat xScale, jfloat yScale, jfloat rotate,
        jdouble toneH, jdouble toneS, jdouble toneV, jdouble contrast,
        jint blurSize, jint blurErode, jlong baseAddr, jlong maskAddr) {

    CForeground f = CForeground();
    f.id = id;
    f.xAxis = xAxis;
    f.yAxis = yAxis;
    f.zIndex = zIndex;
    f.xScale = xScale;
    f.yScale = yScale;
    f.rotate = rotate;
    f.toneH = toneH;
    f.toneS = toneS;
    f.toneV = toneV;
    f.contrast = contrast;
    f.blurSize = blurSize;
    f.blurErode = blurErode;

    auto *baseImage = (Mat4b *) baseAddr;
    auto *maskImage = (Mat1b *) maskAddr;

    lcpp.createMaskedImage(f.maskedImage, &f, *baseImage, *maskImage);

    lcpp.addForeground(f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extUpdateForeground(
        JNIEnv *env, jobject obj,
        jlong id, jint zIndex, jfloat xAxis, jfloat yAxis,
        jfloat xScale, jfloat yScale, jfloat rotate,
        jdouble toneH, jdouble toneS, jdouble toneV, jdouble contrast,
        jint blurSize, jint blurErode) {

    lcpp.updateForeground(id, zIndex, xAxis, yAxis,
                          xScale, yScale, rotate,
                          toneH, toneS, toneV, contrast,
                          blurSize, blurErode);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extUpdateForegroundImage(
        JNIEnv *env, jobject obj,
        jlong targetId, jlong baseAddr, jlong maskAddr) {

    auto baseImage = (Mat4b *) baseAddr;
    auto mask = (Mat1b *) maskAddr;

    CForeground *fg = lcpp.takeForegroundFromId(targetId);
    Mat4b maskedImage;
    lcpp.createMaskedImage(maskedImage, fg, *baseImage, *mask);


    lcpp.updateForegroundImage(targetId, maskedImage);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extSetBackground(
        JNIEnv *env, jobject obj,
        jlong bgId, jlong bgImgAddr) {

    auto *bgImage = (Mat4b *) bgImgAddr;

    lcpp.setBackground(bgId, *bgImage);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extCreateInitialMask(
        JNIEnv *env, jobject obj,
        jlong srcAddr, jlong destAddr) {

    auto *src = (Mat4b *) srcAddr;
    auto *dest = (Mat1b *) destAddr;

    lcpp.createInitialMask(*src, *dest);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extSetCurrentForeground(
        JNIEnv *env, jobject obj,
        jlong fgId) {

    lcpp.setCurrentForeground(fgId);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extGetThumbnailImage(
        JNIEnv *env, jobject obj,
        jlong destAddr) {

    auto dest = (Mat4b *) destAddr;
    lcpp.getImageForThumbnail(*dest);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extGetSmallThumbnail(
        JNIEnv *env, jobject obj,
        jlong srcAddr, jlong destAddr, jint width, jint height, jfloat offset) {

    auto src = (Mat4b *) srcAddr;
    auto dest = (Mat4b *) destAddr;
    lcpp.getSmallThumbnail(*src, *dest, width, height, offset);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extSetForegroundsOrder(
        JNIEnv *env, jobject obj,
        jintArray order) {

    jint* array = env->GetIntArrayElements(order, nullptr);
    jsize len = env->GetArrayLength(order);
    lcpp.setForegroundsOrder(array, len);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extDeleteBackground(
        JNIEnv *env, jobject obj) {

    lcpp.deleteBackground();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extDetouchForeground(
        JNIEnv *env, jobject obj,
        jlong fgId) {

    lcpp.detouchForeground(fgId);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_flutter_1app3_platform_1view_LayoutEditor_extImageNormalization(
        JNIEnv *env, jobject obj,
        jlong src, jlong dest, jint degree, jint size) {
    auto *srcMat = (Mat4b *) src;
    auto *destMat = (Mat4b *) dest;

    lcpp.imageNormalization(*srcMat, destMat, degree, size);
}