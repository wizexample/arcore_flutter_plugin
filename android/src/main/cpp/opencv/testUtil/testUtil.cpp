//
//  nativeExec.cpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/10/16.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#include <iostream>
#include "testUtil.hpp"

void TestUtil::test(Mat4b src, Mat4b &dest) {
    dest = src.clone();
    Mat1b gray = Mat1b::zeros(src.size());
    Mat1b binary = Mat1b::zeros(src.size());
    cvtColor(src, gray, COLOR_RGBA2GRAY);
    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    binary.forEach([&dest](uchar &p, const int *position) -> void {
        if (p > 0) {
            Vec4b &maskPt = dest.at<Vec4b>(position[0], position[1]);
            maskPt[3] = 0;
        }
    });
}
