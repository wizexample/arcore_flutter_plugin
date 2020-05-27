//
//  layerEditorBridge.h
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/17.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "data.hpp"

NS_ASSUME_NONNULL_BEGIN

@interface layoutEditorBridge : NSObject
- (void) createProject: (int)_id name:(NSString*)_name bgId:(int)_bgId imgId:(int)_imgId uped:(bool)_uped thumb:(NSString*)_thumb fId:(int)_fId;
- (void) close;
- (void) addForeground: (int)_id z:(int)_z x:(double)_x y:(double)_y xScale:(double)_xs yScale:(double)_ys r:(double)_r toneH:(double)_h toneS:(double)_s toneV:(double)_v contrast:(double)_c blurSize:(int)_blurSize blurErode:(int)_blurErode img:(UIImage *)_img mask:(UIImage *)_mask;
- (void) detouchForeground: (int)_targetId;
- (void) setCurrentForeground: (int)_fgId;
- (UIImage *) getDisplayImage;
- (UIImage *) getImageForThumbnail;
- (UIImage *) getSmallThumbnail: (UIImage*)_src w:(int)width h:(int)height offset:(float)offset;
- (void) setBackground: (int)_bgId img:(UIImage *)_img;
- (void) deleteBackground;
- (void) setForegroundsOrder: (NSArray*)_order;

- (void) updateForeground: (int)_id z:(int)_z x:(double)_x y:(double)_y xScale:(double)_xs yScale:(double)_ys r:(double)_r toneH:(double)_h toneS:(double)_s toneV:(double)_v contrast:(double)_c blurSize:(int)_blur blurErode:(int)_erode;
- (void) updateForegroundImage: (int)_id img:(UIImage *)_img mask:(UIImage *)_mask;

- (UIImage *) createInitialMask: (UIImage *)_base;
- (UIImage *) createBlankImage: (CGFloat)_w h:(CGFloat)_h;
- (UIImage *) imageNormalization: (UIImage*)_src;
- (UIImage *) imageNormalization: (UIImage*)_src size:(int)_size;

@end

NS_ASSUME_NONNULL_END
