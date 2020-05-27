//
//  bgErase.cpp
//  test
//
//  Created by 上江洲　智久 on 2019/08/01.
//  Copyright © 2019 上江洲　智久. All rights reserved.
//
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
//#include <android/log.h>

using namespace cv;
#include "bgErase.hpp"
#include "../matrixUtil.hpp"

//#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
//#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)

#define USE_BLUR true
//#define PRINT_PROCESSING_TIME

// BgErase

void BgErase::applyMask(const Mat4b &src, const Mat4b &bg, const Mat1b &mask, Mat4b dest, Mat4b transparent) {
#if USE_BLUR
    Mat1b tempMask = mask.clone();
    MatrixUtil::applyBlur(&tempMask, blurSize, blurErode);
    src.forEach([this, bg, tempMask, &dest, &transparent](Vec4b &p, const int *position) -> void {
        uchar maskValue = tempMask(position[0], position[1]);
#else
    src.forEach([this, bg, mask, &dest, &transparent](Vec4b &p, const int *position) -> void {
        int maskValue = mask(position[0], position[1]);
#endif
    
        Vec4b bgPos = bg(position[0], position[1]);
        Vec4b &tpPos = (Vec4b&)transparent(position[0], position[1]);
        Vec4b &destPos = (Vec4b&)dest(position[0], position[1]);
        
        tpPos = p;
        tpPos[3] = maskValue;
        if (maskValue == 0xFF) {
            destPos = p;
#if USE_BLUR
        } else if (maskValue == 0x00) {
            destPos = bgPos;
        } else {
            float mv = (float)maskValue / 0xFF;
            float revMv = (1 - mv);
            destPos[0] = p[0] * mv + bgPos[0] * revMv;
            destPos[1] = p[1] * mv + bgPos[1] * revMv;
            destPos[2] = p[2] * mv + bgPos[2] * revMv;
            destPos[3] = 0xFF;
        }
#else
        } else {
            destPos = bgPos;
        }
#endif
    });
#if USE_BLUR
        tempMask.release();
#endif
}

void BgErase::init(Mat4b *srcMat) {
//    log("BgErase::init");
    blurSize = DEF_BLUR_SIZE;
    blurErode = DEF_BLUR_ERODE;

    Size imageSize = srcMat->size();
    background = Mat4b(imageSize);
    unified = Mat4b(imageSize);
    mask = Mat1b(imageSize);
    MatrixUtil::createCheckerBg(&background);
}

void BgErase::close() {
    background.release();
    base.release();
    unified.release();
    mask.release();
    reset();
}
                
void BgErase::reset() {
    tempH = 0;
    tempS = 0;
    tempV = 0;
    for (int i = 0; i < CHROMAKEY_SIZE; i ++) {
        Chromakey *currentChromakey = &chromakeys[i];
        (currentChromakey->fontColorRgb) = 0;
        currentChromakey->targetHsv = 0;
        currentChromakey->targetRgb = 0;
        currentChromakey->hRange = 0;
        currentChromakey->sRange = 0;
        currentChromakey->vRange = 0;
        MatrixUtil::createHMap(0, 0, currentChromakey->hMap);
        MatrixUtil::createSVMap(0, 0, currentChromakey->sMap);
        MatrixUtil::createSVMap(0, 0, currentChromakey->vMap);
    }
}


uchar BgErase::setChromakeysPointer(int pointer) {
    chromakeyPointer = pointer % CHROMAKEY_SIZE;

    // temporary implementation
    Chromakey *currentChromakey = &(chromakeys[chromakeyPointer]);
    if (currentChromakey->hRange != tempH) {
        MatrixUtil::createHMap(currentChromakey->targetHsv[0], tempH, currentChromakey->hMap);
        currentChromakey->hRange = tempH;
    }
    if (currentChromakey->sRange != tempS) {
        MatrixUtil::createSVMap(currentChromakey->targetHsv[1], tempS, currentChromakey->sMap);
        currentChromakey->sRange = tempS;
    }
    if (currentChromakey->vRange != tempV) {
        MatrixUtil::createSVMap(currentChromakey->targetHsv[2], tempV, currentChromakey->vMap);
        currentChromakey->vRange = tempV;
    }

    return chromakeyPointer;
}

int BgErase::setChromaKey(int vr, int vg, int vb) {
    Vec3b tempHsv;
    MatrixUtil::vecRGBtoHSV(vr, vg, vb, &tempHsv);
    return setChromakeyTarget(tempHsv);
}

void BgErase::drawPointer(float px, float py, float vWidth, float vHeight) {
    float imgRate = max((base.size().width / vWidth), (base.size().height / vHeight));
    float leftMargin = (vWidth  * imgRate - base.size().width) / 2;
    float topMargin = (vHeight * imgRate - base.size().height) / 2;
    prevCoordinate = Point(px * imgRate - leftMargin, py * imgRate - topMargin);
}

int BgErase::setChromaKeyPixel(float px, float py, float vWidth, float vHeight) {
    Point point = MatrixUtil::calcScaledTouchPoint(px, py, vWidth, vHeight, base.size(), 1, 0, 0);
    if (point.x < 0 || point.y < 0 || point.x > base.size().width || point.y > base.size().height) {
        return -1;
    }
    Vec4b pixel = base(point.y, point.x);
    drawPointer(px, py, vWidth, vHeight);
    return setChromaKey(pixel[0], pixel[1], pixel[2]);
}

int BgErase::setChromaKeyColor(float h, float s, float v) {
    Vec3b tempHsv;
    tempHsv[0] = h;
    tempHsv[1] = s;
    tempHsv[2] = v;
    
    return setChromakeyTarget(tempHsv);
}

int BgErase::setChromakeyTarget(Vec3b hsv) {
    Chromakey *currentChromakey = &(chromakeys[chromakeyPointer]);
    Vec3b &targetHsv = currentChromakey->targetHsv;
    if (targetHsv[0] != hsv[0]) {
        MatrixUtil::createHMap(hsv[0], currentChromakey->hRange, currentChromakey->hMap);
    }
    if (targetHsv[1] != hsv[1]) {
        MatrixUtil::createSVMap(hsv[1], currentChromakey->sRange, currentChromakey->sMap);
    }
    if (targetHsv[2] != hsv[2]) {
        MatrixUtil::createSVMap(hsv[2], currentChromakey->vRange, currentChromakey->vMap);
    }
    targetHsv = hsv;
    currentChromakey->targetRgb = MatrixUtil::vecHSVtoRGB(hsv[0], hsv[1], hsv[2]);
    currentChromakey->fontColorRgb = MatrixUtil::inverseColor(currentChromakey->targetRgb);

    return ((targetHsv[0] & 0xFF) << 16) | ((targetHsv[1] & 0xFF) << 8) | (targetHsv[2] & 0xFF);
}

void BgErase::setChromaKeyRange(float h, float s, float v) {
    // temporary implmentation
    tempH = h;
    tempS = s;
    tempV = v;
        
    Chromakey *currentChromakey = &(chromakeys[chromakeyPointer]);
    Vec3b &targetHsv = currentChromakey->targetHsv;
    if (currentChromakey->hRange != h) {
        MatrixUtil::createHMap(targetHsv[0], h, currentChromakey->hMap);
        currentChromakey->hRange = h;
    }
    if (currentChromakey->sRange != s) {
        MatrixUtil::createSVMap(targetHsv[1], s, currentChromakey->sMap);
        currentChromakey->sRange = s;
    }
    if (currentChromakey->vRange != v) {
        MatrixUtil::createSVMap(targetHsv[2], v, currentChromakey->vMap);
        currentChromakey->vRange = v;
    }
}

void BgErase::toneAdjust(float h, float s, float v, float c) {
//    log2("toneAdjust %f %f %f %f", h , s, v, c);
    toneH = h;
    toneS = s;
    toneV = v;
    contrast = c;
}

void BgErase::setBlurParams(int size, int erode) {
    blurSize = size;
    blurErode = erode;
}

long testsum;
int testcounter;

//double rotateArr[2][3] = {{0, 1, 0},{-1, 0, 0}};
//const Mat rotateMat = cv::Mat(2, 3, CV_64F, rotateArr);

void BgErase::getDisplayImage(Mat4b *srcMat, Mat &outputMat) {

#ifdef PRINT_PROCESSING_TIME
    timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
#endif
    ///////////////////////////
    base.release();

    Size srcSize = srcMat->size();
    outputMat = Mat4b(srcSize);
    base = *srcMat;

    Mat4b workerBase = Mat4b(srcSize);
    mask = 0xFF;

    Mat3b hsv;
    cvtColor(*srcMat, hsv, COLOR_RGB2HSV);
//    MatrixUtil::decolor(*srcMat, &hsv);
    for (int i = 0; i < CHROMAKEY_SIZE; i ++) {
        Chromakey currentChromakey = chromakeys[i];
        MatrixUtil::createChromakeyMask(hsv, &mask, currentChromakey.hMap, currentChromakey.sMap, currentChromakey.vMap, false);
    }
    MatrixUtil::applyToneShift(&hsv, &hsv, toneH, toneS, toneV);
    cvtColor(hsv, hsv, COLOR_HSV2RGB);
    cvtColor(hsv, workerBase, COLOR_RGB2RGBA);
    MatrixUtil::applyContrast(&workerBase, &workerBase, contrast);
    hsv.release();


    applyMask(workerBase, background, mask, outputMat, unified);

    if (isShowPickedColor) {
        Chromakey currentChromakey = chromakeys[chromakeyPointer];
        circle(outputMat, prevCoordinate, SIZE_COLOR_CIRCLE, currentChromakey.targetRgb, -1);
        String text = std::to_string(chromakeyPointer + 1);
        Size textSize = getTextSize(text, FONT_HERSHEY_COMPLEX, FONTSIZE_COLOR_NUMBER, FONT_THICKNESS, 0);
        Size textDraw = Size(prevCoordinate.x - textSize.width / 2, prevCoordinate.y + textSize.height / 2);
        putText(outputMat, text, textDraw , FONT_HERSHEY_COMPLEX, FONTSIZE_COLOR_NUMBER, currentChromakey.fontColorRgb, FONT_THICKNESS);
    }

    workerBase.release();

    ////////////////////////
#ifdef PRINT_PROCESSING_TIME
    timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    testsum += ((end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec));
    testcounter ++;
    if (testcounter >= 10) {
        std::cout << "sum: " << testsum << ", counter: " << testcounter << std::endl;
        testsum = 0;
        testcounter = 0;
    }
#endif
}

void BgErase::getBaseImage(Mat4b &dest) {
    dest = base;
}

void BgErase::getMaskImage(Mat1b &dest) {
    dest = mask;
}

void BgErase::getUnifiedImage(Mat4b &dest) {
    dest = unified;
}

void BgErase::getSmallThumbnail(Mat4b &dest, int width, int height, float offset) {
    dest = Mat4b(width, height);
    MatrixUtil::resize(unified, dest, Size(width, height), offset);
}

uchar BgErase::changeBg(uchar pattern) {
    return MatrixUtil::changeBg(&background, pattern);
}

void BgErase::showPickedColor() {
    isShowPickedColor = true;
}

void BgErase::hidePickedColor() {
    isShowPickedColor = false;
}
