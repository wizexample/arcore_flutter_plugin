//
//  matrixUtil.hpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/27.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#ifndef matrixUtil_hpp
#define matrixUtil_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;

class MatrixUtil {
private:
    static constexpr uchar BG_PATTERN_SIZE = 3;
    static double prevToneH, prevToneS, prevToneV, prevContrast;
    static Mat3b toneLUT;
    static Mat4b contrastLUT;
    static Mat3b decolorLUT;
    static bool decolorLUTPrepared;

public:
    static void reset();
    
    static void applyToneShift(const Mat3b *hsvSrc, Mat3b *hsvDest, double h, double s, double v);
    static void applyContrast(const Mat4b *argbSrc, Mat4b *argbDest, double contrast);
    static void createCheckerBg(Mat4b *dest);
    static uchar changeBg(Mat4b *background, uchar pattern);
    static void applyBlur(Mat1b *mask, int size, int erodeItr);
    static void bilateralBlur(const Mat4b& base, Mat3b *dest);

    static Mat makeAffineMatrix(Size size, double xScale, double yScale, double rotate, double xAxis, double yAxis);
    static void vecRGBtoHSV(int r, int g, int b, Vec3b* dest);
    static Scalar vecHSVtoRGB(int h, int s, int v);
    static Scalar inverseColor(Scalar rgb);
    
    static Point calcScaledTouchPoint(float px, float py, float vWidth, float vHeight, const Size& imgSize, double scale, double shiftLeft, double shiftTop);
    static void makeInitialMask(Mat4b *src, Mat1b *destMask);


    static void createChromakeyMask(const Mat3b hsv, Mat1b *mask, bool *hMap, bool *sMap, bool *vMap, bool isInverse);
    static void createChromakeyMask2(const Mat3b hsv, Mat1b *mask, bool *hMap, bool *sMap, bool *vMap);
    static void createHMap(int targetH, int hRange, bool *hMap);
    static void createSVMap(int target, int range, bool *map);
    
    static void resizeCenter(const Mat4b& src, Mat4b *dst, Size size);
    static void resize(const Mat4b& src, Mat4b &dst, Size size, float offset = -1);
    static void imageNormalization(const Mat4b& src, Mat4b *dest, int degree, int longSideLength = 0);
    
    static void makeEdgeMatrix(Mat4b base, Mat1b *edge);
    static void decolor(Mat4b src, Mat3b* dest);
    static long currentTime();
};

#endif /* matrixUtil_hpp */
