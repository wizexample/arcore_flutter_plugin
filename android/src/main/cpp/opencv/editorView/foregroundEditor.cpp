//
//  foregroundEditor.cpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/24.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#include "foregroundEditor.hpp"
#include "../matrixUtil.hpp"

#ifdef ANDROID

#include <android/log.h>
#include <utility>

#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)
#else
#define log(a) printf(a)
#define log2(a, ...) printf(a, __VA_ARGS__)
#endif

const Scalar ForegroundEditor::POINTER_COLOR = Scalar(0xFF, 0x00, 0x00, 0xFF);
const Scalar ForegroundEditor::FINGER_COLOR = Scalar(0xFF, 0x00, 0x00, 0x33);

void ForegroundEditor::open(const Mat4b &base, Mat1b mask) {
    long start = MatrixUtil::currentTime();
    openTime = start;

    this->base = base;
    this->oldHistoriesMask = std::move(mask);
    base.copyTo(toneShiftedBase);

    scale = 1.0;
    shiftLeft = 0;
    shiftTop = 0;

    chromakeyRangeH = 15.75;
    chromakeyRangeS = 71.4;
    chromakeyRangeV = 120;

    blurSize = 0;
    blurErode = 0;

    prevCoordinate = cv::Point(-100, -100);
    eraserWidth = DEF_ERASER_WIDTH;
    eraserOffset = DEF_ERASER_OFFSET;

    historyPointer = -1;
    size = this->base.size();
    background = Mat4b(size);
    unitedMask = oldHistoriesMask.clone();
    edge = Mat1b(size);
    blurredRGB = Mat3b(size);
    MatrixUtil::makeEdgeMatrix(this->base, &edge);
    MatrixUtil::bilateralBlur(this->base, &blurredRGB);

    MatrixUtil::createCheckerBg(&background);
    if (openTime == start) {
        isReady = true;
    }
}

void ForegroundEditor::onViewResized(float width, float height) {
    viewSize = Size(width, height);
    if (!isReady) {
        return;
    }
    float defScaleW = width / size.width;
    float defScaleH = height / size.height;
    defaultScale = min(defScaleH, defScaleW);
    float leftMargin = (width - size.width * defaultScale) / 2;
    float topMargin = (height - size.height * defaultScale) / 2;
    defaultShiftLeft = (1.0f - defaultScale) * size.width / 2 - leftMargin;
    defaultShiftTop = (1.0f - defaultScale) * size.height / 2 - topMargin;
}

void ForegroundEditor::setBackground(const Mat4b &bg) {
    MatrixUtil::resize(bg, projectBg, base.size(), 0.5);
    background.release();
    background = projectBg.clone();
}

void ForegroundEditor::close() {
    openTime = MatrixUtil::currentTime();
    isReady = false;
    background.release();
    base.release();
    toneShiftedBase.release();
    projectBg.release();
    edge.release();
    blurredRGB.release();
    clearHistory();
}

void ForegroundEditor::updateMaskImage(Mat1b mask) {
    clearHistory();

    oldHistoriesMask = mask;
    unitedMask = oldHistoriesMask.clone();
}

void ForegroundEditor::clearHistory() {
    currentMask.release();
    unitedMask.release();
    oldHistoriesMask.release();
    for (auto itr = histories.begin(); itr != histories.end(); ++itr) {
        (*itr).mask.release();
    }
    histories.clear();
    historyPointer = -1;
    currentHistory.isChromakey = false;
    currentHistory.isErase = true;
}

float ForegroundEditor::setScale(float scale) {
    float maxValue = MAX_DISP_SCALE;
    float minValue = MIN_DISP_SCALE;
    scale = std::min(maxValue, std::max(minValue, scale));
    this->scale = scale;
    return this->scale;
}

void ForegroundEditor::setShifts(float dLeft, float dTop) {
    this->shiftLeft += dLeft;
    this->shiftTop += dTop;
}

void ForegroundEditor::setPosition(float x, float y) {
    this->shiftLeft = x;
    this->shiftTop = y;
}


void ForegroundEditor::toneAdjust(float h, float s, float v, float contrast) {
    Mat3b hsv = Mat3b(size);
    cvtColor(base, hsv, COLOR_RGB2HSV);

    MatrixUtil::applyToneShift(&hsv, &hsv, h, s, v);

    cvtColor(hsv, hsv, COLOR_HSV2RGB);
    cvtColor(hsv, toneShiftedBase, COLOR_RGB2RGBA);

    MatrixUtil::applyContrast(&toneShiftedBase, &toneShiftedBase, contrast);

    hsv.release();
}

void ForegroundEditor::setBlurParams(int size, int erode) {
    blurSize = size;
    blurErode = erode;
}

void ForegroundEditor::getDisplayImage(Mat4b &dest) {
    if (!isReady) {
        return;
    }
    Mat4b temp = Mat4b(size);
    Mat1b tempMask;
    getMaskImage(tempMask);

    MatrixUtil::applyBlur(&tempMask, blurSize, blurErode);

    temp.forEach([this, tempMask](Vec4b &p, const int *position) -> void {
        int maskValue = tempMask(position[0], position[1]);
        Vec4b basePos = toneShiftedBase(position[0], position[1]);
        Vec4b bgPos = background(position[0], position[1]);

        if (maskValue == 0xFF) {
            p = basePos;
        } else if (maskValue == 0x00) {
            p = bgPos;
        } else {
            float mv = (float) maskValue / 0xFF;
            float revMv = (1 - mv);
            p[0] = basePos[0] * mv + bgPos[0] * revMv;
            p[1] = basePos[1] * mv + bgPos[1] * revMv;
            p[2] = basePos[2] * mv + bgPos[2] * revMv;
            p[3] = 0xFF;
        }
    });
    tempMask.release();

    dest = Mat4b(viewSize.width, viewSize.height);
    float displayScale = defaultScale * scale;

    // limit shifting image
    float leftLimit = viewSize.width * 0.9;
    float topLimit = viewSize.height * 0.9;
    float leftOffset = (viewSize.width - size.width * displayScale) / 2;
    float topOffset = (viewSize.height - size.height * displayScale) / 2;
    if (shiftLeft > leftLimit - leftOffset) {
        shiftLeft = leftLimit - leftOffset;
    } else if (shiftLeft < -(size.width * displayScale - viewSize.width * 0.1) - leftOffset) {
        shiftLeft = -(size.width * displayScale - viewSize.width * 0.1) - leftOffset;
    }
    if (shiftTop > topLimit - topOffset) {
        shiftTop = topLimit - topOffset;
    } else if (shiftTop < -(size.height * displayScale - viewSize.height * 0.1) - topOffset) {
        shiftTop = -(size.height * displayScale - viewSize.height * 0.1) - topOffset;
    }

    Mat affineMat = MatrixUtil::makeAffineMatrix(size, displayScale, displayScale, 0.0,
                                                 shiftLeft - defaultShiftLeft,
                                                 shiftTop - defaultShiftTop);
    cv::warpAffine(temp, dest, affineMat, viewSize);
    temp.release();

    if (touchMode == TOUCHMODE_DRAW) {
        circle(dest, prevCoordinate, SIZE_FINGER_POINTER, FINGER_COLOR, -1);
        Point offsetCoordinate = Point(prevCoordinate.x,
                                       prevCoordinate.y - eraserOffset * defaultScale);
        circle(dest, offsetCoordinate, (eraserWidth / 2) * displayScale, POINTER_COLOR, 5);
    }

    if ((touchMode == TOUCHMODE_CHROMAKEY || touchMode == TOUCHMODE_FILL_RANGE) &&
        isShowPickedColor) {
        circle(dest, prevCoordinate, SIZE_COLOR_CIRCLE, targetRgb, -1);
    }
}

void ForegroundEditor::getMaskImage(Mat1b &dest) {
    dest = unitedMask.clone();
    if (historyPointer >= 0) {
        combineHistories(currentHistory, &dest);
    }
}

int ForegroundEditor::setTouchMode(int mode) {
    chromaOrFillPixelSelected = false;
    this->touchMode = mode;
    return this->touchMode;
}

void ForegroundEditor::setDrawWidth(int width) {
    eraserWidth = width;
}

void ForegroundEditor::setDrawOffset(int offset) {
    eraserOffset = offset;
}

void ForegroundEditor::drawMask(float px, float py, float vWidth, float vHeight, int lineWidth,
                                bool isErase, bool isStart) {
    Point current = MatrixUtil::calcScaledTouchPoint(px, py, vWidth, vHeight, size, scale,
                                                     shiftLeft / defaultScale,
                                                     shiftTop / defaultScale);
    current.y -= eraserOffset / scale;
    float lw = lineWidth;
    if (isStart) {
        createNewHistory(isErase, false);
        circle(currentMask, current, lw / 2, 0xFF, -1);
    } else {
        line(currentMask, prevPoint, current, 0xFF, lw);
    }

    eraserWidth = lineWidth;
    prevPoint = current;
}

void ForegroundEditor::drawPointer(float px, float py, float vWidth, float vHeight) {
//    float imgRate = max((base.size().width / vWidth), (base.size().height / vHeight));
//    float leftMargin = (vWidth  * imgRate - base.size().width) / 2;
//    float topMargin = (vHeight * imgRate - base.size().height) / 2;
//    prevCoordinate = Point(px * imgRate - leftMargin, py * imgRate - topMargin);
    prevCoordinate = Point(px, py);
}

int
ForegroundEditor::setChromaKeyPixel(float px, float py, float vWidth, float vHeight, bool start) {
    if (start) {
        createNewHistory(true, true);
    }
    Point point = MatrixUtil::calcScaledTouchPoint(px, py, vWidth, vHeight, size, scale,
                                                   shiftLeft / defaultScale,
                                                   shiftTop / defaultScale);
    if (point.x < 0 || point.y < 0 || point.x > size.width || point.y > size.height) {
        return -1;
    }
    chromaOrFillPixelSelected = true;
    Vec3b pixel = blurredRGB(point.y, point.x);
    drawPointer(px, py, vWidth, vHeight);
    return setChromaKey(pixel[0], pixel[1], pixel[2]);

//    Vec3b pixel = decoloredHSV(point.y, point.x);
//    return setChromakeyTarget(pixel);
}

void ForegroundEditor::setChromaKeyRange(float h, float s, float v) {
    if (chromakeyRangeH != h) {
        MatrixUtil::createHMap(targetHsv[0], h, hMap);
        chromakeyRangeH = h;
    }
    if (chromakeyRangeS != s) {
        MatrixUtil::createSVMap(targetHsv[1], s, sMap);
        chromakeyRangeS = s;
    }
    if (chromakeyRangeV != v) {
        MatrixUtil::createSVMap(targetHsv[2], v, vMap);
        chromakeyRangeV = v;
    }
    if (currentHistory.isChromakey) {
        createColorMask();
    }
}

int ForegroundEditor::setChromaKeyColor(float h, float s, float v) {
    Vec3b tempHsv;
    tempHsv[0] = h;
    tempHsv[1] = s;
    tempHsv[2] = v;

    return setChromakeyTarget(tempHsv);
}

int ForegroundEditor::setChromaKey(int r, int g, int b) {
    Vec3b tempHsv;
    MatrixUtil::vecRGBtoHSV(r, g, b, &tempHsv);
    return setChromakeyTarget(tempHsv);
}

int ForegroundEditor::setChromakeyTarget(Vec3b hsv) {
    if (targetHsv[0] != hsv[0]) {
        MatrixUtil::createHMap(hsv[0], chromakeyRangeH, hMap);
    }
    if (targetHsv[1] != hsv[1]) {
        MatrixUtil::createSVMap(hsv[1], chromakeyRangeS, sMap);
    }
    if (targetHsv[2] != hsv[2]) {
        MatrixUtil::createSVMap(hsv[2], chromakeyRangeV, vMap);
    }
    targetHsv = hsv;
    targetRgb = MatrixUtil::vecHSVtoRGB(hsv[0], hsv[1], hsv[2]);

    createColorMask();
    return ((targetHsv[0] & 0xFF) << 16) | ((targetHsv[1] & 0xFF) << 8) | (targetHsv[2] & 0xFF);

}

void ForegroundEditor::createColorMask() {
    if (!chromaOrFillPixelSelected) {
        return;
    }
    Mat3b hsv = Mat3b(base.size());
    cvtColor(blurredRGB, hsv, COLOR_RGB2HSV);
    MatrixUtil::createChromakeyMask2(hsv, &currentMask, hMap, sMap, vMap);
    hsv.release();

//    MatrixUtil::createChromakeyMask2(decoloredHSV, &currentMask, hMap, sMap, vMap);
}

int
ForegroundEditor::setFloodFillPixel(float px, float py, float vWidth, float vHeight, bool start) {
    if (start) {
        createNewHistory(true, true);
    }
    Point tempPoint = MatrixUtil::calcScaledTouchPoint(px, py, vWidth, vHeight, size, scale,
                                                       shiftLeft / defaultScale,
                                                       shiftTop / defaultScale);

    if (tempPoint.x < 0 || tempPoint.y < 0 || tempPoint.x >= size.width - 2 ||
        tempPoint.y >= size.height - 2) {
        cout << "out of point" << endl;
        return -1;
    }
    chromaOrFillPixelSelected = true;
    floodFillTargetPoint = tempPoint;

    drawPointer(px, py, vWidth, vHeight);

    Vec4b tempVec = base.at<Vec4b>(floodFillTargetPoint.y, floodFillTargetPoint.x);
    targetRgb = Scalar(tempVec[0], tempVec[1], tempVec[2], 0xFF);
    executeFloodFill();

    return 0;
}

void ForegroundEditor::setFloodFillRange(float r, float g, float b) {
    regionRangeR = r;
    regionRangeG = g;
    regionRangeB = b;
    if (currentHistory.isChromakey) {
        executeFloodFill();
    }
}

void ForegroundEditor::executeFloodFill() {
    if (!chromaOrFillPixelSelected ||
        (regionRangeR == 0.0f && regionRangeG == 0.0f && regionRangeB == 0.0f)) {
        return;
    }
    Mat1b tempMask = Mat1b::zeros(size.height + 2, size.width + 2);

    Vec3b diff = Vec3b(regionRangeR / 2, regionRangeG / 2, regionRangeB / 2);
    Mat1b roi = tempMask(Rect(1, 1, size.width, size.height));
    bitwise_not(unitedMask, roi);

    floodFill(blurredRGB, tempMask, floodFillTargetPoint, 0, 0, diff, diff, FLAG_FLOODFILL);
    tempMask(Rect(1, 1, size.width, size.height)).copyTo(currentMask);
    tempMask.release();

//    currentMask = Mat1b::zeros(size);
//
//    Mat3b temp;
//    temp = blurredRGB(Rect(1, 1, size.width - 2, size.height - 2));
//    Vec3b diff = Vec3b(regionRangeR / 2, regionRangeG / 2, regionRangeB / 2);
//
//    floodFill(temp, currentMask, floodFillTargetPoint, 0, 0, diff, diff, FLAG_FLOODFILL);
//    temp.release();
}

void ForegroundEditor::createNewHistory(bool isErase, bool isChromakey) {
    killFollowingHistories();
    currentHistory = OperationHistory();
    currentHistory.mask = Mat1b::zeros(size);
    currentHistory.isErase = isErase | isChromakey;
    currentHistory.isChromakey = isChromakey;
    histories.push_back(currentHistory);
    currentMask = currentHistory.mask;
    // サイズを超えた場合
    if (histories.size() > HISTORY_SIZE) {
        OperationHistory pops = histories.at(0);
        histories.erase(histories.begin());
        combineHistories(pops, &oldHistoriesMask);
        pops.mask.release();
    }
    historyForward();
}


void ForegroundEditor::killFollowingHistories() {
    for (int i = (int) histories.size() - 1; i > historyPointer; i--) {
        OperationHistory pops = histories.at(i);
        pops.mask.release();
        histories.pop_back();
    }
}

void ForegroundEditor::combineHistories(OperationHistory src, Mat1b *dest) {
    int val = src.isChromakey || src.isErase ? 0x00 : 0xFF;
    Mat1b add = src.mask;

    (*dest).forEach([val, add](unsigned char &p, const int *position) -> void {
        if (add.at<unsigned char>(position[0], position[1]) == 0xFF) {
            p = val;
        }
    });
}

void ForegroundEditor::createUnitedMask() {
    unitedMask.release();
    cout << "ForegroundEditor::createUnitedMask: histories:" << histories.size() << "current:"
         << historyPointer << endl;
    unitedMask = oldHistoriesMask.clone();
    // currentより前のHistoryを結合する(currentは含まない)
    for (int i = 0; i < historyPointer; i++) {
        OperationHistory history = histories.at(i);
        combineHistories(history, &unitedMask);
    }
}

void ForegroundEditor::historyBack() {
    historyPointer--;
    historyPointer = max(-1, historyPointer);
    if (historyPointer >= 0) {
        currentHistory = histories.at(historyPointer);
    }
    createUnitedMask();
}

void ForegroundEditor::historyForward() {
    historyPointer++;
    historyPointer = min((int) histories.size() - 1, historyPointer);
    if (historyPointer >= 0) {
        currentHistory = histories.at(historyPointer);
    }
    createUnitedMask();
}

void ForegroundEditor::historyFirst() {
    historyPointer = -1;
    createUnitedMask();
}

void ForegroundEditor::historyLast() {
    historyPointer = (int) histories.size() - 1;
    if (historyPointer >= 0) {
        currentHistory = histories.at(historyPointer);
    }
    createUnitedMask();
}

void ForegroundEditor::cancelLastHistory() {
    historyPointer--;
    historyPointer = max(-1, historyPointer);
    if (historyPointer >= 0) {
        currentHistory = histories.at(historyPointer);
    }
    if (histories.size() > 0) {
        histories.pop_back();
    }
    createUnitedMask();

}

// 先頭2byteがredoできる数/後ろ2byteがundoできる数
int ForegroundEditor::historyCount() {
    int prevCount = historyPointer + 1;
    int forwardCount = (int) histories.size() - historyPointer - 1;
    return ((forwardCount & 0xFF) << 8) | (prevCount & 0xFF);
}

uchar ForegroundEditor::changeBg(uchar pattern) {
    if (hasProjectBg && pattern % 4 == 3) {
        background.release();
        background = projectBg.clone();
        return 3;
    }
    return MatrixUtil::changeBg(&background, pattern);
}

void ForegroundEditor::showPickedColor() {
    isShowPickedColor = true;
}

void ForegroundEditor::hidePickedColor() {
    isShowPickedColor = false;
}

void ForegroundEditor::setMaskAll(uchar alpha) {
    createNewHistory(alpha == 0xFF, false);
}
