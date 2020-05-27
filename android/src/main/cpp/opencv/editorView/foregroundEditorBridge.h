//
//  foregroundEditorBridge.h
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/24.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface foregroundEditorBridge : NSObject

- (void) open: (UIImage *)_base mask:(UIImage *)_mask background:(nullable UIImage *)_bg;
- (void) close;
- (void) onViewResized:(CGFloat)_width height:(CGFloat)_height;
- (void) updateMaskImage: (UIImage *)_mask;
- (CGFloat) setScale: (CGFloat)_scale;
- (void) setShifts: (CGFloat)_dLeft top:(CGFloat)_dTop;
- (void) setPosition: (float)x y:(float)y;
- (void) toneAdjust: (float)_h s:(float)_s v:(float)_v contrast:(float)_c;
- (void) setBlurParams: (int)_size erode:(int)_erode;
- (UIImage *) getDisplayImage;
- (UIImage *) getMaskImage;
//- (void) drawMask: (CGFloat)_xRatio yRatio:(CGFloat)_yRatio lineWidth:(int)_width isErase:(bool)_erase isStart:(bool)_isStart;
- (int) setTouchMode: (int)_mode;
- (void) setDrawWidth: (int)_width;
- (void) setDrawOffset: (int)_offset;
- (void) drawMask: (CGPoint)_point viewSize:(CGRect)_size lineWidth:(int)_width isErase:(bool)_erase isStart:(bool)_isStart;
- (void) drawPointer: (CGPoint)_point viewSize:(CGRect)_size;
- (void) setChromaKey: (int)_r g:(int)_g b:(int)_b;
- (int) setChromaKeyPixel: (CGPoint)_point viewSize:(CGRect)_size start:(Boolean)_start;
- (void) setChromaKeyRange: (float)h s:(float) s v:(float) v;
- (int) setChromaKeyColor: (float)h s:(float) s v:(float) v;
- (int) setFloodFillPixel: (CGPoint)_point viewSize:(CGRect)_size start:(Boolean)_start;
- (void) setFloodFillRange: (float)r g:(float) g b:(float) b;

- (void) historyBack;
- (void) historyForward;
- (void) historyFirst;
- (void) historyLast;
- (int) historyCount;
- (void) cancelLastHistory;

- (int) changeBg: (int) pattern;

- (void) showPickedColor;
- (void) hidePickedColor;

- (UIImage *) createBlankImage: (CGFloat)_w h:(CGFloat)_h;
- (void) setMaskAll:(int) alpha;

@end

NS_ASSUME_NONNULL_END
