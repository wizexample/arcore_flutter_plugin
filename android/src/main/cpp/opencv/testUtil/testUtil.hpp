//
//  testUtil.hpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/10/16.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#ifndef testUtil_hpp
#define testUtil_hpp
#include <opencv2/opencv.hpp>

#include <stdio.h>
using namespace cv;

class TestUtil {
  
public:
    void test(Mat4b src, Mat4b &dest);
};

#endif /* testUtil_hpp */
