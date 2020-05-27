//
//  foregroundEditorBridge.m
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/24.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#import <opencv2/opencv.hpp>
#import <opencv2/highgui.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <opencv2/imgproc/imgproc_c.h>
#import "foregroundEditorBridge.h"
#import "foregroundEditor.hpp"
#import "../data.hpp"

@implementation foregroundEditorBridge

ForegroundEditor fgCpp = ForegroundEditor();

- (void) open: (UIImage *)_base mask:(UIImage *)_mask background:(nullable UIImage *)_bg {
    fgCpp.open([self importImage:_base], [self importMask:_mask]);
    if (_bg != NULL) {
        fgCpp.setBackground([self importImage:_bg]);
    }
}

- (void) close {
    fgCpp.close();
}

- (void) onViewResized:(CGFloat)_width height:(CGFloat)_height {
    fgCpp.onViewResized(_width, _height);
}

- (void) updateMaskImage:(UIImage *)_mask {
    fgCpp.updateMaskImage([self importMask: _mask]);
}

- (CGFloat) setScale: (CGFloat)_scale {
    return fgCpp.setScale(_scale);
}

- (void) setShifts:(CGFloat)_dLeft top:(CGFloat)_dTop {
    fgCpp.setShifts(_dLeft, _dTop);
}

- (void) setPosition: (float)x y:(float)y {
    fgCpp.setPosition(x, y);
}

- (void) toneAdjust: (float)_h s:(float)_s v:(float)_v contrast:(float)_c {
    fgCpp.toneAdjust(_h, _s, _v, _c);
}

- (void) setBlurParams: (int)_size erode:(int)_erode {
    fgCpp.setBlurParams(_size, _erode);
}

- (UIImage *) getDisplayImage {
    Mat4b img;
    fgCpp.getDisplayImage(img);
    UIImage *resultImg = MatToUIImage(img);
    return resultImg;
}

- (UIImage *) getMaskImage {
    Mat1b img;
    fgCpp.getMaskImage(img);
    UIImage *resultImg = MatToUIImage(img);
    return resultImg;
}

- (int) setTouchMode:(int)_mode {
    return fgCpp.setTouchMode(_mode);
}

- (void) setDrawWidth:(int)_width {
    fgCpp.setDrawWidth(_width);
}

- (void) setDrawOffset:(int)_offset {
    fgCpp.setDrawOffset(_offset);
}

- (void) drawMask: (CGPoint)_point viewSize:(CGRect)_size lineWidth:(int)_width isErase:(bool)_erase isStart:(bool)_isStart {
    fgCpp.drawMask(_point.x, _point.y, _size.size.width, _size.size.height, _width, _erase, _isStart);
}

- (void) drawPointer: (CGPoint)_point viewSize:(CGRect)_size {
    fgCpp.drawPointer(_point.x, _point.y, _size.size.width, _size.size.height);
}

- (void) setChromaKey: (int)_r g:(int)_g b:(int)_b {
    NSLog(@"rgb: %d, %d, %d", _r, _g, _b);
    fgCpp.setChromaKey(_r, _g, _b);
}

- (int) setChromaKeyPixel: (CGPoint)_point viewSize:(CGRect)_size start:(Boolean)_start {
    return fgCpp.setChromaKeyPixel(_point.x, _point.y, _size.size.width, _size.size.height, _start);
}

- (void) setChromaKeyRange: (float) h s:(float) s v:(float) v {
    fgCpp.setChromaKeyRange(h, s, v);
}

- (int) setChromaKeyColor: (float) h s:(float) s v:(float) v {
    return fgCpp.setChromaKeyColor(h, s, v);
}

- (int) setFloodFillPixel: (CGPoint)_point viewSize:(CGRect)_size start:(Boolean)_start {
    return fgCpp.setFloodFillPixel(_point.x, _point.y, _size.size.width, _size.size.height, _start);
}

- (void) setFloodFillRange: (float) r g:(float) g b:(float) b {
    fgCpp.setFloodFillRange(r, g, b);
}


- (UIImage *) createBlankImage: (CGFloat)_w h:(CGFloat)_h {
    Mat1b ret = Mat1b::ones(_h, _w) | 0xFF;
    return MatToUIImage(ret);
}

- (void) historyBack {
    fgCpp.historyBack();
}

- (void) historyForward {
    fgCpp.historyForward();
}

- (void) historyFirst {
    fgCpp.historyFirst();
}

- (void) historyLast {
    fgCpp.historyLast();
}

- (int) historyCount {
    return fgCpp.historyCount();
}

- (void) cancelLastHistory {
    fgCpp.cancelLastHistory();
}

- (int) changeBg:(int)pattern {
    return fgCpp.changeBg(pattern);
}

- (void) showPickedColor {
    fgCpp.showPickedColor();
}

- (void) hidePickedColor {
    fgCpp.hidePickedColor();
}

- (void) setMaskAll:(int) alpha {
    fgCpp.setMaskAll(alpha);
}


//////////////////////////////////////////////////
- (Mat4b) importImage: (UIImage *)img {
    CGFloat cols = img.size.width;
    CGFloat rows = img.size.height;
    Mat4b mat(rows, cols, CV_8UC4);
    UIImageToMat(img, mat);
    return mat;
}

- (Mat1b) importMask: (UIImage *)mask {
    CGFloat cols = mask.size.width;
    CGFloat rows = mask.size.height;
    Mat1b mat(rows, cols, CV_8UC1);
    UIImageToMat(mask, mat);
    return mat;
}
@end
