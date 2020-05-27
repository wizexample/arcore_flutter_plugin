//
//  nativeExec.hpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/10/16.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#ifndef nativeExec_hpp
#define nativeExec_hpp
#include <opencv2/opencv.hpp>

#include <stdio.h>
using namespace cv;

class NativeExec {
  
public:
    void makeThumbnail(Mat4b src, Mat4b &dest, int width, int height, float offset);
    void makeInitialMask(Mat4b src, Mat1b &dest);
    void imageNormalization(Mat4b src, Mat4b &dest, int degree, int size);
};

#endif /* nativeExec_hpp */
