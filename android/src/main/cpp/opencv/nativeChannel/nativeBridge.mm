//
//  NativeBridge.m
//  Runner
//
//  Created by 上江洲　智久 on 2019/10/16.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#import <opencv2/opencv.hpp>
#import <opencv2/highgui.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <opencv2/imgproc/imgproc_c.h>
#import "nativeBridge.h"
#import "nativeExec.hpp"

@implementation NativeBridge

NativeExec ncpp = NativeExec();

- (UIImage *) makeThumbnail: (UIImage*)_src w:(int)width h:(int)height offset:(float)offset {
    Mat4b ret;
    ncpp.makeThumbnail([self importImage: _src], ret, width, height, offset);
    return MatToUIImage(ret);
}

- (UIImage *) makeInitialMask:(UIImage *)_src {
    Mat1b ret;
    ncpp.makeInitialMask([self importImage: _src], ret);
    return MatToUIImage(ret);
}

- (UIImage *) imageNormalization:(UIImage *)_src {
    return [self imageNormalization:_src size:0];
}

- (UIImage *) imageNormalization:(UIImage *)_src size:(int)_size {
    int degree = 0;
    switch (_src.imageOrientation) {
        case UIImageOrientationDown:
            degree = 180;
            break;
        case UIImageOrientationLeft:
            degree = 90;
            break;
        case UIImageOrientationRight:
            degree = 270;
            break;
        default:
            break;
    }
    Mat4b ret;
    ncpp.imageNormalization([self importImage: _src], ret, degree, _size);
    return MatToUIImage(ret);
}

//////////////////////////////////////////////////
- (Mat4b) importImage: (UIImage *)img {
    CGFloat cols = img.size.width;
    CGFloat rows = img.size.height;
    Mat4b mat(rows, cols, CV_8UC4);
    UIImageToMat(img, mat, true);
    return mat;
}

@end
