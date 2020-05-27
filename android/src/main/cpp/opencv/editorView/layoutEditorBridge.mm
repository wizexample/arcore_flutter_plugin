//
//  layerEditorBridge.m
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/17.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//
#import <opencv2/opencv.hpp>
#import <opencv2/highgui.hpp>
#import <opencv2/imgcodecs/ios.h>
#import <opencv2/imgproc/imgproc_c.h>
#import "layoutEditorBridge.h"
#import "layoutEditor.hpp"
#import "data.hpp"

@implementation layoutEditorBridge
LayoutEditor cpp = LayoutEditor();

- (void) createProject: (int)_id name:(NSString*)_name bgId:(int)_bgId imgId:(int)_imgId uped:(bool)_uped thumb:(NSString*)_thumb fId:(int)_fId {
    // fixme
    CProject p = CProject();
    p.id = _id;
    p.name = (char*)[_name UTF8String];
    p.bgId = _bgId;
    p.mainImageId = _imgId;
    p.uploaded = _uped;
    p.thumbnail = (char*)[_thumb UTF8String];
    p.folderId = _fId;
    
    cpp.initProject(p);
}

- (void) close {
    cpp.close();
}

- (void) addForeground: (int)_id z:(int)_z x:(double)_x y:(double)_y xScale:(double)_xs yScale:(double)_ys r:(double)_r toneH:(double)_h toneS:(double)_s toneV:(double)_v contrast:(double)_c blurSize:(int)_blurSize blurErode:(int)_blurErode img:(UIImage *)_img mask:(UIImage *)_mask{
    CForeground f = CForeground();
    f.id = _id;
    f.xAxis = _x;
    f.yAxis = _y;
    f.zIndex = _z;
    f.xScale = _xs;
    f.yScale = _ys;
    f.rotate = _r;
    f.toneH = _h;
    f.toneS = _s;
    f.toneV = _v;
    f.contrast = _c;
    f.blurSize = _blurSize;
    f.blurErode = _blurErode;
    
    Mat4b baseImage = [self importImage: _img];
    Mat1b maskImage = [self importMask: _mask];
    
    cpp.createMaskedImage(f.maskedImage, &f, baseImage, maskImage);
    
    baseImage.release();
    maskImage.release();
    
    cpp.addForeground(f);
}

- (void) detouchForeground: (int)_targetId {
    cpp.detouchForeground(_targetId);
}

- (void) setCurrentForeground: (int)_fgId {
    cpp.setCurrentForeground(_fgId);
}

- (UIImage *) getDisplayImage {
    Mat4b img;
    cpp.getDisplayImage(img);
    UIImage *resultImg = MatToUIImage(img);
    return resultImg;
}

- (UIImage *) getImageForThumbnail {
    Mat4b img;
    cpp.getImageForThumbnail(img);
    UIImage *resultImg = MatToUIImage(img);
    return resultImg;
}

- (UIImage *) getSmallThumbnail: (UIImage*)_src w:(int)width h:(int)height offset:(float)offset {
    Mat4b input = [self importImage: _src];
    Mat4b dest;
    cpp.getSmallThumbnail(input, dest, width, height, offset);
    UIImage *resultImg = MatToUIImage(dest);
    return resultImg;
}

- (void) setBackground: (int)_bgId img:(UIImage *)_img {
    cpp.setBackground(_bgId, [self importImage: _img]);
}

- (void) deleteBackground {
    cpp.deleteBackground();
}

- (void) setForegroundsOrder: (NSArray*)_order {
    int count = (int)[_order count];
    int order[count];

    for (int i = 0; i < count; ++i) {
        order[i] = [[_order objectAtIndex:i] intValue];
    }
    cpp.setForegroundsOrder(order, count);
}


- (void) updateForeground: (int)_id z:(int)_z x:(double)_x y:(double)_y xScale:(double)_xs yScale:(double)_ys r:(double)_r toneH:(double)_h toneS:(double)_s toneV:(double)_v contrast:(double)_c blurSize:(int)_blur blurErode:(int)_erode {
    cpp.updateForeground(_id, _z, _x, _y, _xs, _ys, _r, _h, _s, _v, _c, _blur, _erode);
}

- (void) updateForegroundImage: (int)_id img:(UIImage *)_img mask:(UIImage *)_mask {
    Mat4b baseImage = [self importImage: _img];
    Mat1b mask = [self importMask: _mask];

    CForeground* fg = cpp.takeForegroundFromId(_id);

    Mat4b maskedImage;
    cpp.createMaskedImage(maskedImage, fg, baseImage, mask);
    
    baseImage.release();
    mask.release();
    
    cpp.updateForegroundImage(_id, maskedImage);
}

- (UIImage *) createInitialMask:(UIImage *)_base {
    Mat1b dest;
    cpp.createInitialMask([self importImage: _base], dest);
    return MatToUIImage(dest);
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
    Mat4b dest;
    cpp.imageNormalization([self importImage: _src], &dest, degree, _size);
    return MatToUIImage(dest);
}


//////////////////////////////////////////////////
- (Mat4b) importImage: (UIImage *)img {
    CGFloat cols = img.size.width;
    CGFloat rows = img.size.height;
    Mat4b mat(rows, cols, CV_8UC4);
    UIImageToMat(img, mat, true);
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
