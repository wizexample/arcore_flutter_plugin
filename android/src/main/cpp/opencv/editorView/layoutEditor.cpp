//
//  layerEditor.cpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/17.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <math.h>
#include "../matrixUtil.hpp"

//#ifndef M_PI
//#define M_PI 3.14159265358979
//#endif
//#define deg2rad(deg) (((deg)/360)*2*M_PI)

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))


using namespace cv;

#include "layoutEditor.hpp"

//#define ANDROID

#ifdef ANDROID
#include <android/log.h>
#define log(a) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a)
#define log2(a, ...) __android_log_print(ANDROID_LOG_DEBUG,"Tag", a, __VA_ARGS__)
#else
#define log(a) printf(a)
#define log2(a, ...) printf(a, __VA_ARGS__)
#endif

static const int X = 0;
static const int Y = 1;

void LayoutEditor::initProject(CProject project) {
    isReady = false;
    this->project = project;

}

void LayoutEditor::close() {
    for (auto itr = this->project.foregrounds.begin();
         itr != this->project.foregrounds.end(); ++itr) {
        itr->maskedImage.release();
    }
    isReady = false;
    project.background.release();
}

void LayoutEditor::addForeground(const CForeground foreground) {
    if (this->project.mainImageId == foreground.id) {
        this->project.canvasSize = foreground.maskedImage.size();
        if (project.bgId < 1) {
            project.background = Mat4b(project.canvasSize);
            MatrixUtil::createCheckerBg(&project.background);
            isReady = true;
        }
    }
    this->project.foregrounds.push_back(foreground);
    project.sort();
}

void LayoutEditor::detouchForeground(int targetId) {
    for (auto itr = this->project.foregrounds.begin();
         itr != this->project.foregrounds.end(); ++itr) {
        if ((*itr).id == targetId) {
            this->project.foregrounds.erase(itr);
            return;
        }
    }
}

void LayoutEditor::setCurrentForeground(int fgId) {
    currentForegroundId = fgId;
}

void LayoutEditor::setBackground(int bgId, Mat4b bgImage) {
    isReady = false;
    this->project.background.release();
    project.bgId = bgId;
    project.background = Mat4b(project.canvasSize);
    MatrixUtil::resizeCenter(bgImage, &project.background, project.canvasSize);
    bgImage.release();
    isReady = true;
}

void LayoutEditor::deleteBackground() {
    isReady = false;
    this->project.background.release();
    project.bgId = 0;
    project.background = Mat4b(project.canvasSize);
    MatrixUtil::createCheckerBg(&project.background);
    isReady = true;
}


void LayoutEditor::createMaskedImage(Mat4b &dest, CForeground *fg, Mat4b base, const Mat1b& mask) {
    Size size = base.size();
    dest = Mat4b::zeros(size);

    Mat3b hsv = Mat3b(size);
    cvtColor(base, hsv, COLOR_RGB2HSV);
    MatrixUtil::applyToneShift(&hsv, &hsv, fg->toneH, fg->toneS, fg->toneV);
    cvtColor(hsv, hsv, COLOR_HSV2RGB);

    Mat1b workerMask = mask.clone();
    MatrixUtil::applyBlur(&workerMask, fg->blurSize, fg->blurErode);

    base.forEach([workerMask, &dest, &hsv](Vec4b &p, const int *position) -> void {
        uchar maskValue = workerMask(position[X], position[Y]);
        Vec4b &destPos = dest.at<Vec4b>(position[X], position[Y]);
        Vec3b &hsvPos = hsv.at<Vec3b>(position[X], position[Y]);

        if (maskValue != 0x00) {
            destPos[0] = hsvPos[0];
            destPos[1] = hsvPos[1];
            destPos[2] = hsvPos[2];
            destPos[3] = maskValue;
        } else {
            destPos[0] = 0;
            destPos[1] = 0;
            destPos[2] = 0;
            destPos[3] = 0;

        }
    });
    hsv.release();
    workerMask.release();

    MatrixUtil::applyContrast(&dest, &dest, fg->contrast);
}

void LayoutEditor::getDisplayImage(Mat4b &dest) {
    if (!isReady) {
        dest = Mat4b();
    } else {
        uniteImages(dest, true, true);
    }
}

void LayoutEditor::getImageForThumbnail(Mat4b &dest) {
    // 背景がある場合のみ背景を埋める 市松模様は描かない
    uniteImages(dest, project.bgId > 0, false);
}

void LayoutEditor::getSmallThumbnail(const Mat4b src, Mat4b &dest, int width, int height, float offset) {
    // cuurent used only from add foreground in layout editor.
    Size s = Size(width, height);
    dest = Mat4b::zeros(s);
    MatrixUtil::resize(src, dest, s, offset);
}


void LayoutEditor::uniteImages(Mat4b &dest, bool addChecker, bool enableHalfTransprent) {
    dest = Mat4b::zeros(this->project.canvasSize);
    this->project.foregrounds.begin();
    // foregrounds はz-index順でソートされている
    for (auto &foreground : this->project.foregrounds) {
        putImage(&dest, foreground, enableHalfTransprent);
    }
//    for (auto itr = this->project.foregrounds.begin();
//              itr != this->project.foregrounds.end(); ++itr) {
//        putImage(&ret, *itr, enableHalfTransprent);
//    }
    if (addChecker) {
        putBackground(&dest);
    }
}

void LayoutEditor::putImage(Mat4b *canvas, const CForeground &image, bool enableHalfTransparent) {
    Size size = image.maskedImage.size();
    Size canvasSize = canvas->size();
    // 出力領域
    Mat4b temp = Mat4b::zeros(canvasSize);

    Mat affineMat = MatrixUtil::makeAffineMatrix(size, image.xScale, image.yScale, image.rotate,
                                                 image.xAxis, image.yAxis);
    cv::warpAffine(image.maskedImage, temp, affineMat, canvasSize);
    // 保存サムネイルじゃない場合 && 操作対象IDが-1じゃない場合 && forで回しているIDが操作対象ではない場合 -> 半透過
    bool halfTransparentLayer =
            enableHalfTransparent && currentForegroundId != -1 && image.id != currentForegroundId;
    // レイヤー上から 透過ピクセルのみ描画対象
    canvas->forEach([&temp, halfTransparentLayer](Vec4b &p, const int *position) -> void {
//        Vec4b destPos = (Vec4b &) temp(position[X], position[Y]);
        Vec4b *destPos = temp.ptr<Vec4b>(position[X], position[Y]);
        if (p[3] == 0x00) {
            p = *destPos;
            if (halfTransparentLayer) {
                p[3] = (uchar) (p[3] * 0.5f);
            }
        } else if (p[3] < 0xFF) {
            float srcA = (float) p[3] / 0xFF;
            float dstA;
            if (halfTransparentLayer) {
                dstA = (float) (*destPos)[3] * 0.5f * (1.F - srcA) / 0xFF;
            } else {
                dstA = (float) (*destPos)[3] * (1.F - srcA) / 0xFF;
            }
            float outA = srcA + dstA;
            p[0] = (uchar) (((*destPos)[0] * dstA + p[0] * srcA) / outA);
            p[1] = (uchar) (((*destPos)[1] * dstA + p[1] * srcA) / outA);
            p[2] = (uchar) (((*destPos)[2] * dstA + p[2] * srcA) / outA);
            p[3] = (uchar) (0xFF * outA);
        }
    });
    temp.release();
}

void LayoutEditor::putBackground(Mat4b *canvas) {
    canvas->forEach([this](Vec4b &p, const int *position) -> void {
        Vec4b &destPos = this->project.background(position[X], position[Y]);
        if (p[3] == 0x00) {
            p = destPos;
        } else if (p[3] < 0xFF) {
            float srcA = (float) p[3] / 0xFF;
            float revSrcA = (1.0F - srcA);
            p[0] = destPos[0] * revSrcA + p[0] * srcA;
            p[1] = destPos[1] * revSrcA + p[1] * srcA;
            p[2] = destPos[2] * revSrcA + p[2] * srcA;
            p[3] = 0xFF;
        }
    });
}

void LayoutEditor::setForegroundsOrder(int *order, int arraySize) {
    int foregroundsLength = (int) project.foregrounds.size();
    // すべて最前面に
    for (auto itr = this->project.foregrounds.begin();
         itr != this->project.foregrounds.end(); ++itr) {
        (*itr).zIndex = foregroundsLength;
    }
    for (int i = 0; i < arraySize; i++) {
        takeForegroundFromId(order[i])->zIndex = i;
    }
    project.sort();
}

void LayoutEditor::updateForeground(int targetId, int z, double x, double y, double xScale,
                                    double yScale, double rotate, double toneH, double toneS,
                                    double toneV, double contrast, int blurSize, int blurErode) {
    CForeground *f = takeForegroundFromId(targetId);
    if (f == nullptr) {
        cout << "LayoutEditor::updateForeground: try to update foreground, but missing id:"
             << targetId << endl;
        return;
    }
    f->zIndex = z;
    f->xAxis = x;
    f->yAxis = y;
    f->xScale = xScale;
    f->yScale = yScale;
    f->rotate = rotate;
    f->toneH = toneH;
    f->toneS = toneS;
    f->toneV = toneV;
    f->contrast = contrast;
    f->blurSize = blurSize;
    f->blurErode = blurErode;
}

void LayoutEditor::updateForegroundImage(int targetId, Mat4b maskedImage) {
    CForeground *f = takeForegroundFromId(targetId);
    f->maskedImage.release();
    f->maskedImage = std::move(maskedImage);
}

CForeground *LayoutEditor::takeForegroundFromId(int targetId) {
    for (auto itr = this->project.foregrounds.begin();
         itr != this->project.foregrounds.end(); ++itr) {
        if ((*itr).id == targetId) {
            return &*itr;
        }
    }
    return nullptr;
}

void LayoutEditor::createInitialMask(Mat4b base, Mat1b &dest) {
    dest = Mat1b::zeros(base.size());
    MatrixUtil::makeInitialMask(&base, &dest);
}

void LayoutEditor::imageNormalization(const Mat4b src, Mat4b *dest, int degree, int size) {
    MatrixUtil::imageNormalization(src, dest, degree, size);
}
