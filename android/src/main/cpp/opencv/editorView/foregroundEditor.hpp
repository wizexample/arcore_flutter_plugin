//
//  foregroundEditor.hpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/24.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#ifndef foregroundEditor_hpp
#define foregroundEditor_hpp

#include <stdio.h>
#include "../data.hpp"

using namespace cv;

class ForegroundEditor {
private:
    static constexpr float DEF_H_VAL = 90.0f;
    static constexpr float DEF_S_VAL = 255.0f;
    static constexpr float DEF_V_VAL = 255.0f;
    static constexpr int DEF_ERASER_WIDTH = 20;
    static constexpr float DEF_ERASER_OFFSET = 200.0f;
    static constexpr uchar BG_PATTERN_SIZE = 3;
    static constexpr int FLAG_FLOODFILL = 4 | 255 << 8 | FLOODFILL_FIXED_RANGE | FLOODFILL_MASK_ONLY;
    
    static constexpr int SIZE_FINGER_POINTER = 15;
    static constexpr int SIZE_COLOR_CIRCLE = 30;
    static constexpr int FONTSIZE_COLOR_NUMBER = 25;
    
    
    static constexpr float MIN_DISP_SCALE = 0.5f;
    static constexpr float MAX_DISP_SCALE = 5.0f;
    
    static constexpr int TOUCHMODE_NONE = 0;
    static constexpr int TOUCHMODE_CHROMAKEY = 1;
    static constexpr int TOUCHMODE_DRAW = 2;
    static constexpr int TOUCHMODE_FILL_RANGE = 3;

    static const Scalar POINTER_COLOR;
    static const Scalar FINGER_COLOR;

    bool isReady = false;
    long openTime;
    cv::Size viewSize;
    float defaultScale = 1.0f;
    float defaultShiftLeft, defaultShiftTop;

    static constexpr int HISTORY_SIZE = 10;
    cv::Point prevPoint = cv::Point(0, 0);
    cv::Point prevCoordinate = cv::Point(-100, -100);
    vector<OperationHistory> histories;
    int historyPointer = -1;
    OperationHistory currentHistory;
    int touchMode;

    Mat4b background;
    bool hasProjectBg;
    Mat4b projectBg;
    Mat1b currentMask; // 現在操作中のマスク 0x00: 未操作 / 0xFF: OperationHistory.isEraseによって削除か復帰になる
    Mat1b unitedMask; // 表示時に計算するコストを省くためのcurrentより古い結合マスク  0x00: 非表示 / 0xFF: 表示 の2値
    Mat1b oldHistoriesMask; // ヒストリーより古い結合マスク 0x00: 非表示 / 0xFF: 表示 の2値
    
    Mat4b base;
    Mat4b toneShiftedBase;
    Mat1b edge;
    Mat3b blurredRGB;
    cv::Size size;
    float scale = 1.0f;
    float shiftLeft = 0.f;
    float shiftTop = 0.f;
    float eraserOffset = DEF_ERASER_OFFSET;
    int eraserWidth = DEF_ERASER_WIDTH;

    Vec3b targetHsv;
    Scalar targetRgb;
    bool chromaOrFillPixelSelected;
    
    cv::Point floodFillTargetPoint;
    float chromakeyRangeH, chromakeyRangeS, chromakeyRangeV;
    bool hMap[180], sMap[256], vMap[256];
    bool isShowPickedColor;
    float regionRangeR, regionRangeG, regionRangeB;
    
    int blurSize, blurErode;
    

    void createColorMask();
    void executeFloodFill();
    
    void createNewHistory(bool isErase, bool isChromakey);
    void killFollowingHistories();
    void combineHistories(OperationHistory src, Mat1b *dest);
    void createUnitedMask();

    void clearHistory();
    int setChromakeyTarget(Vec3b hsv);

public:
    void open(const Mat4b& base, Mat1b mask);
    void onViewResized(float width, float height);
    void setBackground(const Mat4b& bg);
    int setTouchMode(int mode);
    void setDrawWidth(int width);
    void setDrawOffset(int offset);
    void drawMask(float px, float py, float vWidth, float vHeight, int lineWidth, bool isErase, bool isStart);
    void drawPointer(float px, float py, float vWidth, float vHeight);
    float setScale(float scale);
    void setShifts(float dLeft, float dTop);
    void setPosition(float x, float y);
    int setChromaKey(int r, int g, int b);
    int setChromaKeyPixel(float px, float py, float vWidth, float vHeight, bool start);
    void setChromaKeyRange(float h, float s, float v);
    int setChromaKeyColor(float h, float s, float v);
    void setFloodFillRange(float r, float g, float b);
    int setFloodFillPixel(float px, float py, float vWidth, float vHeight, bool start);
    void toneAdjust(float h, float s, float v, float contrast);
    void setBlurParams(int size, int erode);
    void getDisplayImage(Mat4b &dest);
    void getMaskImage(Mat1b &dest);
    void close();
    void updateMaskImage(Mat1b mask);
    
    void historyBack();
    void historyForward();
    void historyFirst();
    void historyLast();
    int historyCount();
    void cancelLastHistory();
    
    uchar changeBg(uchar pattern);
    
    void showPickedColor();
    void hidePickedColor();

    void setMaskAll(uchar alpha);
};

#endif /* foregroundEditor_hpp */
