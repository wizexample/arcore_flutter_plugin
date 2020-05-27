//
//  OpenCV.h
//  Runner
//
//  Created by 上江洲　智久 on 2019/08/01.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface CppBridge : NSObject
- (void) close;
- (int) setChromakeysPointer: (int)_pointer;
- (void) setChromaKey: (int)_r g:(int)_g b:(int)_b;
- (int) setChromaKeyPixel: (CGPoint)_point viewSize:(CGRect)_size;
- (int) setChromaKeyColor: (float)h s:(float) s v:(float) v;
- (void) setChromaKeyRange: (float)h s:(float) s v:(float) v;
- (void) toneAdjust: (float)_h s:(float)_s v:(float)_v contrast:(float)_c;
- (UIImage *) getDisplayImage:(UIImage *)src;
- (void) setBlurParams:  (int)_size erode:(int)_erode;
- (UIImage *) getBaseImage;
- (UIImage *) getMaskImage;
- (UIImage *) getUnifiedImage;
- (UIImage *) getSmallThumbnail: (int)width h:(int)height o:(float)offset;
- (int) changeBg: (int) pattern;
- (void) showPickedColor;
- (void) hidePickedColor;

@end

NS_ASSUME_NONNULL_END
