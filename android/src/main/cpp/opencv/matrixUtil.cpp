//
//  matrixUtil.cpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/27.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#include "matrixUtil.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979
#endif
#define deg2rad(deg) (((deg)/360)*2*M_PI)

#ifdef ANDROID
#include <android/log.h>
#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)
#else
#define log(a) printf(a)
#define log2(a, ...) printf(a, __VA_ARGS__)
#endif

double MatrixUtil::prevToneH = -1;
double MatrixUtil::prevToneS = -1;
double MatrixUtil::prevToneV = -1;
double MatrixUtil::prevContrast = -1;
Mat3b MatrixUtil::toneLUT = Mat3b(1, 256);
Mat4b MatrixUtil::contrastLUT = Mat4b(1, 256);
Mat3b MatrixUtil::decolorLUT = Mat3b(1, 256);
bool MatrixUtil::decolorLUTPrepared = false;

void MatrixUtil::reset() {
    prevToneH = -1;
    prevToneS = -1;
    prevToneV = -1;
    prevContrast = -1;
}

void MatrixUtil::applyToneShift(const Mat3b *hsvSrc, Mat3b *hsvDest, double h, double s, double v) {
    if (h < 0) {
        h += 180;
    }
    if (h != prevToneH) {
        uchar *p = toneLUT.data;
        for (int i = 0; i < 256; ++i) {
            p[i * 3] = ((int) (i + h) % 181);
        }
        prevToneH = h;
    }
    if (s != prevToneS) {
        uchar *p = toneLUT.data;
        for (int i = 0; i < 256; ++i) {
            p[i * 3 + 1] = min((int) (i * s), 0xFF);
        }
        prevToneS = s;
    }
    if (v != prevToneV) {
        uchar *p = toneLUT.data;
        for (int i = 0; i < 256; ++i) {
            p[i * 3 + 2] = min((int) (i * v), 0xFF);
        }
        prevToneV = v;
    }

    LUT(*hsvSrc, toneLUT, *hsvDest);

//    hsvSrc->forEach([hsvDest, h, s, v](Vec3b &p, const int *position) -> void {
//        Vec3b &destPos = hsvDest->at<Vec3b>(position[0], position[1]);
//        destPos[0] = p[0] + h;
//        destPos[1] = p[1] * s;
//        destPos[2] = p[2] * v;
//    });
}

double sigmoid(double contrast, double input) {
    if (contrast == 0) return input;
    else if (contrast > 0)
        return (asinh((2 * input / 0xFF - 1) * sinh(contrast)) / 2 / contrast + 0.5) * 0xFF;
    else return (sinh(contrast * (2 * input / 0xFF - 1)) / 2 / sinh(contrast) + 0.5) * 0xFF;

}

void MatrixUtil::applyContrast(const Mat4b *argbSrc, Mat4b *argbDest, double contrast) {
    if (prevContrast != contrast) {
        double c = (contrast - 5) * 2; // -10 - 10 へ正規化
        for (int i = 0; i < 256; i++) {
            double sig = sigmoid(c, i);
            contrastLUT.data[i * 4] = sig;
            contrastLUT.data[i * 4 + 1] = sig;
            contrastLUT.data[i * 4 + 2] = sig;
            contrastLUT.data[i * 4 + 3] = i;
        }
        prevContrast = contrast;
    }
    LUT(*argbSrc, contrastLUT, *argbDest);
}


void MatrixUtil::createCheckerBg(Mat4b *dest) {
    for (int y = 0; y < dest->rows; ++y) {
        for (int x = 0; x < dest->cols; ++x) {
            auto color = static_cast<uchar>(16 * ((x / 20 + y / 20) % 2) + 232);
            Vec4b &destPos = dest->at<Vec4b>(y, x);
            destPos[0] = color;
            destPos[1] = color;
            destPos[2] = color;
            destPos[3] = 255;
        }
    }
}

#define ERODE_BEFORE true
#define BLUR_TYPE 1

void MatrixUtil::applyBlur(Mat1b *mask, int size, int erodeItr) {
    if (size < 1) {
        return;
    }
    int p1 = 3;
    int p2 = 3;
    int p3 = 3;
    // todo: make cv params into this subroutine's param if needed.

#if ERODE_BEFORE
    if (erodeItr > 0) {
        erode(*mask, *mask, Mat(), Point(-1, -1), erodeItr);
    }
#endif

#if BLUR_TYPE == 1
    blur(*mask, *mask, Size(size, size));
#elif BLUR_TYPE == 2
    Mat temp;
    boxFilter(*mask, temp, p1, Size(size, size), Point(-1, -1), p2 != 0);
    temp.copyTo(*mask); // boxFilter mask -> mask は落ちる
    temp.release();
#elif BLUR_TYPE == 3
    medianBlur(*mask, *mask, p1);
#elif BLUR_TYPE == 4
    GaussianBlur(*mask, *mask, Size(size, size), p1, p2);
#elif BLUR_TYPE == 5
    Mat temp;
    bilateralFilter(*mask, temp, p1, p2, p3);
    temp.copyTo(*mask); // bilateralFilter mask -> mask は落ちる
    temp.release();
#endif

#if (!ERODE_BEFORE)
    if (erodeItr > 0) {
        erode(*mask, *mask, Mat(), Point(-1, -1), erodeItr);
    }
#endif
}

#define BIRATERAL_PARAMS 9, 75, 75

void MatrixUtil::bilateralBlur(const Mat4b& base, Mat3b *dest) {
    Mat3b temp;
    cvtColor(base, temp, COLOR_RGBA2RGB);
    bilateralFilter(temp, *dest, BIRATERAL_PARAMS);
    temp.release();
}

Mat
MatrixUtil::makeAffineMatrix(Size size, double xScale, double yScale, double rotate, double xAxis,
                             double yAxis) {
    // todo せん断 -> 拡大縮小 -> 回転 -> 並行移動
    // 拡大縮小・回転の事前処理/事後処理
    double hw = size.width / 2;
    double hh = size.height / 2;
    double preMoveArr[3][3] = {{1, 0, -hw},
                               {0, 1, -hh},
                               {0, 0, 1}};
    const Mat preMoveMat = cv::Mat(3, 3, CV_64F, preMoveArr);
    double postMoveArr[3][3] = {{1, 0, hw},
                                {0, 1, hh},
                                {0, 0, 1}};
    const Mat postMoveMat = cv::Mat(3, 3, CV_64F, postMoveArr);

    // 拡大縮小
    double scaleArray[3][3] = {{xScale, 0,      0},
                               {0,      yScale, 0},
                               {0,      0,      1}};
    const Mat scaleMat = cv::Mat(3, 3, CV_64F, scaleArray);

    // 回転
    double rad = deg2rad(rotate);
    double cosv = cos(rad);
    double sinv = sin(rad);
    double rotateArr[3][3] = {{cosv,  sinv, 0},
                              {-sinv, cosv, 0},
                              {0,     0,    1}};
    const Mat rotateMat = cv::Mat(3, 3, CV_64F, rotateArr);

    Mat affineMat = postMoveMat * rotateMat * scaleMat * preMoveMat;

    // 平行移動
    affineMat.at<double>(0, 2) += xAxis;
    affineMat.at<double>(1, 2) += yAxis;

    affineMat = affineMat.rowRange(0, 2);

    return affineMat;
}

void MatrixUtil::vecRGBtoHSV(int r, int g, int b, Vec3b *dest) {
    Mat3b col = Mat3b(Vec3b(b, g, r));
    Mat3b hsvVec = Mat3b(1, 1);
    cvtColor(col, hsvVec, COLOR_BGR2HSV);
    *dest = hsvVec.at<Vec3b>(0, 0);
    col.release();
    hsvVec.release();
}

Scalar MatrixUtil::vecHSVtoRGB(int h, int s, int v) {
    Mat3b hsv = Mat3b(Vec3b(h, s, v));
    Mat3b rgb = Mat3b(1, 1);
    cvtColor(hsv, rgb, COLOR_HSV2RGB);
    Vec3b res = rgb.at<Vec3b>(0, 0);
    hsv.release();
    rgb.release();
    return Scalar(res[0], res[1], res[2], 0xFF);
}

Scalar MatrixUtil::inverseColor(Scalar rgb) {
    uchar color = (rgb[0] + rgb[1] + rgb[2]) / 3 > 0x80 ? 0x00 : 0xFF;
    return Scalar(color, color, color, rgb[3]);
}


Point MatrixUtil::calcScaledTouchPoint(float px, float py, float vWidth, float vHeight,
                                       const Size &imgSize, double scale, double shiftLeft,
                                       double shiftTop) {
    float imgWidth = imgSize.width;
    float imgHeight = imgSize.height;
    float imgRate = max((imgWidth / vWidth), (imgHeight / vHeight));
    double common = (1 - scale) / 2;

    int x = int(
            ((px * imgRate - (vWidth * imgRate - imgWidth) / 2) - shiftLeft - (imgWidth * common)) /
            scale);
    int y = int(((py * imgRate - (vHeight * imgRate - imgHeight) / 2) - shiftTop -
                 (imgHeight * common)) / scale);

    return Point{x, y};
}

void MatrixUtil::makeInitialMask(Mat4b *src, Mat1b *destMask) {
    src->forEach([destMask](Vec4b &p, const int *position) -> void {
        destMask->at<uchar>(position[0], position[1]) = p[3];
    });
}

void
MatrixUtil::createChromakeyMask(const Mat3b hsv, Mat1b *mask, bool *hMap, bool *sMap, bool *vMap,
                                bool isInverse) {
    uchar targetPixel = isInverse ? 0xFF : 0x00; // マスク対象ピクセル
    uchar untargetPixel = isInverse ? 0x00 : 0xFF;

    hsv.forEach([&mask, hMap, sMap, vMap, targetPixel, untargetPixel](Vec3b &p,
                                                                      const int *position) -> void {
        auto &maskPt = mask->at<uchar>(position[0], position[1]);
        if (maskPt == untargetPixel) {
            if (hMap[p[0]] && sMap[p[1]] && vMap[p[2]]) {
                maskPt = targetPixel;
            } else {
                maskPt = untargetPixel;
            }
        }
    });
}

void
MatrixUtil::createChromakeyMask2(const Mat3b hsv, Mat1b *mask, bool *hMap, bool *sMap, bool *vMap) {
    uchar targetPixel = 0xFF; // マスク対象ピクセル
    uchar untargetPixel = 0x00;

    hsv.forEach([&mask, hMap, sMap, vMap, targetPixel, untargetPixel](Vec3b &p,
                                                                      const int *position) -> void {
        uchar &maskPt = mask->at<uchar>(position[0], position[1]);
        if (hMap[p[0]] && sMap[p[1]] && vMap[p[2]]) {
            maskPt = targetPixel;
        } else {
            maskPt = untargetPixel;
        }
    });
}

void MatrixUtil::createHMap(int targetH, int hRange, bool *hMap) {
    if (hRange <= 0) {
        for (int i = 0; i < 180; i++) {
            hMap[i] = false;
        }
        return;
    }

    int lowerlimit = targetH - hRange;
    int upperlimit = targetH + hRange;
    if (lowerlimit < 0) {
        // 0 -> upperlimit, (180 + lowerlimit) -> 180
        for (int i = 0; i < 180; i++) {
            hMap[i] = (i < upperlimit || (180 + lowerlimit) < i);
        }
    } else if (upperlimit > 180) {
        // lowerlimit -> 180, 0 -> (upperlimit - 180)
        for (int i = 0; i < 180; i++) {
            hMap[i] = (lowerlimit < i || (upperlimit - 180) > i);
        }
    } else {
        // lowerlimit -> upperlimit
        for (int i = 0; i < 180; i++) {
            hMap[i] = (lowerlimit < i && upperlimit > i);
        }
    }
}

void MatrixUtil::createSVMap(int target, int range, bool *map) {
    if (range <= 0) {
        for (int i = 0; i < 256; i++) {
            map[i] = false;
        }
        return;
    }

    int lowerlimit = target - range;
    int upperlimit = target + range;
    for (int i = 0; i < 256; i++) {
        map[i] = (lowerlimit < i && upperlimit > i);
    }
}

void MatrixUtil::resizeCenter(const Mat4b& src, Mat4b *dst, Size size) {
    float scale = max((float) (size.width + 5) / src.size().width,
                      (float) (size.height + 5) / src.size().height);
    int marginX = ((src.size().width * scale) - (size.width + 5) ) / 2;
    int marginY = ((src.size().height * scale) - (size.height + 5) ) / 2;
    double scaleArray[3][3] = {{scale, 0,     0},
                               {0,     scale, 0},
                               {0,     0,     1}};
    Mat affineMat = cv::Mat(3, 3, CV_64F, scaleArray);
    affineMat.at<double>(0, 2) -= marginX;
    affineMat.at<double>(1, 2) -= marginY;
    affineMat = affineMat.rowRange(0, 2);

    cv::warpAffine(src, *dst, affineMat, size);
    affineMat.release();
}

void MatrixUtil::resize(const Mat4b& src, Mat4b &dst, Size size, float offset) {
    float scale;
    int marginX, marginY;
    if (offset < 0 || offset > 1) {
        // scaling as its aspect ratio
        scale = min((float) size.width / src.size().width, (float) size.height / src.size().height);
        marginX = 0;
        marginY = 0;
        size = Size(src.size().width * scale, src.size().height * scale);
        std::cout << "size:: " << (src.size().width * scale) << " , " << (src.size().height * scale)
                  << std::endl;
        dst = Mat4b::zeros(size);
    } else {
        // scaling with cut out square
        scale = max((float) size.width / src.size().width, (float) size.height / src.size().height);
        marginX = ((src.size().width * scale) - size.width) * offset;
        marginY = ((src.size().height * scale) - size.height) * offset;
    }
    double scaleArray[3][3] = {{scale, 0,     0},
                               {0,     scale, 0},
                               {0,     0,     1}};
    Mat affineMat = cv::Mat(3, 3, CV_64F, scaleArray);
    affineMat.at<double>(0, 2) -= marginX;
    affineMat.at<double>(1, 2) -= marginY;
    affineMat = affineMat.rowRange(0, 2);

    cv::warpAffine(src, dst, affineMat, size);
    affineMat.release();

}

uchar MatrixUtil::changeBg(Mat4b *background, uchar pattern) {
    pattern = pattern % BG_PATTERN_SIZE;
    switch (pattern) {
        case 1: {
            Vec4b black;
            black[0] = 0x00;
            black[1] = 0x00;
            black[2] = 0x00;
            black[3] = 0xFF;
            background->forEach([black](Vec4b &p, const int *position) -> void {
                p = black;
            });
        }
            break;
        case 2: {
            Vec4b white;
            white[0] = 0xFF;
            white[1] = 0xFF;
            white[2] = 0xFF;
            white[3] = 0xFF;
            background->forEach([white](Vec4b &p, const int *position) -> void {
                p = white;
            });
        }
            break;
        case 0:
        default: {
            MatrixUtil::createCheckerBg(background);
        }
            break;

    }
    return pattern;
}

/// Exifの回転を画像に適用と取り込みリサイズ
void MatrixUtil::imageNormalization(const Mat4b& src, Mat4b *dest, int degree, int longSideLength) {
    Size srcSize = src.size();

    float scale = 1.0;
    if (longSideLength > 0) {
        float srcLongSide = max(srcSize.width, srcSize.height);
        scale = min(longSideLength / srcLongSide, 1.0f);
    }

    Size destSize;
    double offsetX = 0.0;
    double offsetY = 0.0;
    if (degree == 90 || degree == 270) {
        destSize = Size(srcSize.height * scale, srcSize.width * scale);
        offsetX = -(srcSize.width - destSize.width) / 2.0 - 1;
        offsetY = -(srcSize.height - destSize.height) / 2.0;
    } else {
        destSize = Size(srcSize.width * scale, srcSize.height * scale);
        offsetX = -(srcSize.width - destSize.width) / 2.0;
        offsetY = -(srcSize.height - destSize.height) / 2.0;
    }

    Mat rotateAffine = MatrixUtil::makeAffineMatrix(srcSize, scale, scale, degree, offsetX,
                                                    offsetY);
    *dest = Mat4b::zeros(destSize);
    warpAffine(src, *dest, rotateAffine, destSize);
}


#define EDGE_TYPE 2
#define GRAY_SCALE false
#define SOBEL_PARAMS 1, 1, 3
#define LAPLACIAN_PARAMS 5
#define CANNY_PARAMS 60.0, 180.0, 3

void MatrixUtil::makeEdgeMatrix(Mat4b base, Mat1b *edge) {
#if GRAY_SCALE
    // glayscale
    Mat1b gray = Mat1b(base.size());
    cvtColor(base, gray, COLOR_RGBA2GRAY);
#else
    // color channels
    Mat1b r, g, b;
    Mat1b channels[4];
    split(base, channels);
#endif

#if EDGE_TYPE == 0
    // sobel
    Mat1b x, y;
#if GRAY_SCALE
    cv::Sobel(gray, x, CV_8U, 1, 0);
    cv::Sobel(gray, y, CV_8U, 0, 1);
    *edge = (abs(x) + abs(y)) / 2.0;
#else
    cv::Sobel(channels[0], x, CV_8U, 1, 0);
    cv::Sobel(channels[0], y, CV_8U, 0, 1);
    r = (abs(x) + abs(y)) / 2.0;
    cv::Sobel(channels[1], x, CV_8U, 1, 0);
    cv::Sobel(channels[1], y, CV_8U, 0, 1);
    g = (abs(x) + abs(y)) / 2.0;
    cv::Sobel(channels[2], x, CV_8U, 1, 0);
    cv::Sobel(channels[2], y, CV_8U, 0, 1);
    b = (abs(x) + abs(y)) / 2.0;
    *edge = max(max(r, g), b);
#endif
    x.release();
    y.release();
#elif EDGE_TYPE == 1
    // laplacian
#if GRAY_SCALE
    cv::Laplacian(gray, *edge, CV_8U, LAPLACIAN_PARAMS);
    abs(*edge);
#else
    cv::Laplacian(channels[0], r, CV_8U, LAPLACIAN_PARAMS);
    r = abs(r);
    cv::Laplacian(channels[1], g, CV_8U, LAPLACIAN_PARAMS);
    g = abs(g);
    cv::Laplacian(channels[2], b, CV_8U, LAPLACIAN_PARAMS);
    b = abs(b);
    *edge = max(max(r, g), b);
#endif
#elif EDGE_TYPE == 2
    // canny
#if GRAY_SCALE
    cv::Canny(gray, *edge, CANNY_PARAMS);
#else
    cv::Canny(channels[0], r, CANNY_PARAMS);
    cv::Canny(channels[1], g, CANNY_PARAMS);
    cv::Canny(channels[2], b, CANNY_PARAMS);
    bitwise_or(r, g, *edge);
    bitwise_or(*edge, b, *edge);
#endif
#endif

    //    threshold(*edge, *edge, 100, 255, THRESH_BINARY_INV);
#if GRAY_SCALE
    gray.release();
#else
    r.release();
    channels[0].release();
    g.release();
    channels[1].release();
    b.release();
    channels[2].release();
    channels[3].release();
#endif
}

#define DECOLOR_PARAM 25

void MatrixUtil::decolor(Mat4b src, Mat3b *destHSV) {
    if (!decolorLUTPrepared) {
        uchar *p = decolorLUT.data;
        for (int i = 0; i < 256; ++i) {
            int value = ((int) (i / DECOLOR_PARAM)) * DECOLOR_PARAM;
            p[i * 3] = value;
            p[i * 3 + 1] = value;
            p[i * 3 + 2] = value;
        }
        decolorLUTPrepared = true;
    }
    Mat3b hsv;
    cvtColor(src, hsv, COLOR_RGB2HSV);
    LUT(hsv, decolorLUT, *destHSV);
    hsv.release();
}

long MatrixUtil::currentTime() {
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}