//
//  OpenCV.m
//  Runner
//
//  Created by 上江洲　智久 on 2019/08/01.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//
#import <opencv2/opencv.hpp>
#import <opencv2/highgui.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <opencv2/imgproc/imgproc_c.h>
#import "cppBridge.h"
#import "bgErase.hpp"

@implementation CppBridge
int width, height = 0;
int x, y = -1;
BgErase bgErase = BgErase();

- (void) close {
    bgErase.close();
    width = 0; height = 0;
    x = -1; y = -1;
}

- (int) setChromakeysPointer: (int)_pointer {
    return bgErase.setChromakeysPointer(_pointer);
}

- (void) setChromaKey: (int)_r g:(int)_g b:(int)_b {
    bgErase.setChromaKey(_r, _g, _b);
}

- (int) setChromaKeyPixel: (CGPoint)_point viewSize:(CGRect)_size {
    return bgErase.setChromaKeyPixel(_point.x, _point.y, _size.size.width, _size.size.height);
}

- (int) setChromaKeyColor: (float) h s:(float) s v:(float) v {
    return bgErase.setChromaKeyColor(h, s, v);
}

- (void) setChromaKeyRange: (float) h s:(float) s v:(float) v {
    bgErase.setChromaKeyRange(h, s, v);
}

- (void) toneAdjust: (float)_h s:(float)_s v:(float)_v contrast:(float)_c{
    bgErase.toneAdjust(_h, _s, _v, _c);
}

- (UIImage *) getDisplayImage: (UIImage *)src {
    // *************** UIImage -> cv::Mat変換 ***************
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(src.CGImage);
    CGFloat cols = src.size.width;
    CGFloat rows = src.size.height;
    
    cv::Mat4b mat(rows, cols, CV_8UC4);
    
    CGContextRef contextRef = CGBitmapContextCreate(mat.data,
                                                    cols,
                                                    rows,
                                                    8,
                                                    mat.step[0],
                                                    colorSpace,
                                                    kCGImageAlphaNoneSkipLast |
                                                    kCGBitmapByteOrderDefault);
    
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), src.CGImage);
    CGContextRelease(contextRef);
    
    if (width <= 0 && height <= 0) {
        // 最初のフレームで初期化
        cv::Size size = mat.size();
        width = size.width;
        height = size.height;
        
        bgErase.init(&mat);
    }
    // *************** 処理 ***************
    cv::Mat grayImg;
    bgErase.getDisplayImage(&mat, grayImg);

    // *************** cv::Mat → UIImage ***************
    UIImage *resultImg = MatToUIImage(grayImg);
    return resultImg;
}


- (void) setBlurParams: (int)_size erode:(int)_erode {
    bgErase.setBlurParams(_size, _erode);
}

- (UIImage *) getBaseImage {
    Mat4b mat;
    bgErase.getBaseImage(mat);
    return MatToUIImage(mat);
}

- (UIImage *) getMaskImage {
    Mat1b mat;
    bgErase.getMaskImage(mat);
    return MatToUIImage(mat);
}

- (UIImage *) getUnifiedImage {
    Mat4b mat;
    bgErase.getUnifiedImage(mat);
    return MatToUIImage(mat);
}

- (UIImage *) getSmallThumbnail: (int)width h:(int)height o:(float)offset {
    Mat4b mat;
    bgErase.getSmallThumbnail(mat, width, height, offset);
    return MatToUIImage(mat);
}

- (int) changeBg:(int)pattern {
    return bgErase.changeBg(pattern);
}

- (void) showPickedColor {
    bgErase.showPickedColor();
}

- (void) hidePickedColor {
    bgErase.hidePickedColor();
}


@end
