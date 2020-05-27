//
//  bgErase.hpp
//  test
//
//  Created by 上江洲　智久 on 2019/08/01.
//  Copyright © 2019 上江洲　智久. All rights reserved.
//

#ifndef bgErase_hpp
#define bgErase_hpp

#include <stdio.h>
using namespace cv;

class Chromakey {
public:
    Vec3b targetHsv;
    Scalar targetRgb;
    Scalar fontColorRgb;
    float hRange, sRange, vRange;
    bool hMap[180], sMap[256], vMap[256];
    
};

class BgErase {
private:
    static constexpr uchar CHROMAKEY_SIZE = 3;
    static constexpr int DEF_BLUR_SIZE = 0;
    static constexpr int DEF_BLUR_ERODE = 0;
    static constexpr int SIZE_COLOR_CIRCLE = 30;
    static constexpr double FONTSIZE_COLOR_NUMBER = 1.8;
    static constexpr int FONT_THICKNESS = 2;

    float tempH, tempS, tempV; // for temporary implementation

    float toneH = 0;
    float toneS = 1;
    float toneV = 1;
    float contrast = 5;
    Mat4b background;
    Mat4b base;
    Mat4b unified;
    Mat1b mask;
    
    uchar chromakeyPointer = 0;
    Chromakey chromakeys[CHROMAKEY_SIZE];
    bool isShowPickedColor;
    cv::Point prevCoordinate = cv::Point(-100, -100);
    int blurSize, blurErode;

    void applyMask(const Mat4b &src, const Mat4b &bg, const Mat1b &mask, Mat4b dest, Mat4b transparent);
    int setChromakeyTarget(Vec3b hsv);
    void reset();
    
public:
    void init(Mat4b *srcMat);
    void close();
    uchar setChromakeysPointer(int pointer);
    int setChromaKey(int vr, int vg, int vb);
    int setChromaKeyPixel(float px, float py, float vWidth, float vHeight);
    int setChromaKeyColor(float h, float s, float v);
    void setChromaKeyRange(float h, float s, float v);
    void drawPointer(float px, float py, float vWidth, float vHeight);
    void toneAdjust(float h, float s, float v, float c);
    void setBlurParams(int size, int erode);
    void getDisplayImage(Mat4b *srcMat, Mat &destMat);
    void getMaskImage(Mat1b &dest);
    void getBaseImage(Mat4b &dest);
    void getUnifiedImage(Mat4b &dest);
    void getSmallThumbnail(Mat4b &dest, int width, int height, float offset);
    uchar changeBg(uchar pattern);
    void showPickedColor();
    void hidePickedColor();

};
#endif /* bgErase_hpp */
