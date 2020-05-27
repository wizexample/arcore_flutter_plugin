//
//  NativeBridge.h
//  Runner
//
//  Created by 上江洲　智久 on 2019/10/16.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface NativeBridge : NSObject

- (UIImage *) makeThumbnail: (UIImage *)_src w:(int)width h:(int)height offset:(float)offset;

- (UIImage *) makeInitialMask: (UIImage*)_src;

- (UIImage *) imageNormalization: (UIImage*)_src;
- (UIImage *) imageNormalization: (UIImage*)_src size:(int)_size;

@end

NS_ASSUME_NONNULL_END
