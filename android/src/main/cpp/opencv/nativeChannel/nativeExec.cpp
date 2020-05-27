//
//  nativeExec.cpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/10/16.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#include "nativeExec.hpp"
#include "../matrixUtil.hpp"

void NativeExec::makeThumbnail(Mat4b src, Mat4b &dest, int width, int height, float offset) {
//    Mat4b ret = Mat4b(width, height);
    dest = Mat4b(src.size());
    MatrixUtil::resize(src, dest, Size(width, height), offset);
//    return ret;
}

void NativeExec::makeInitialMask(Mat4b src, Mat1b &dest) {
//    Mat1b ret = Mat1b(src.size());
    dest = Mat1b(src.size());
    MatrixUtil::makeInitialMask(&src, &dest);
//    return ret;
}

void NativeExec::imageNormalization(Mat4b src, Mat4b &dest, int degree, int size) {
    dest = Mat4b(src.size());
    MatrixUtil::imageNormalization(src, &dest, degree, size);
}
