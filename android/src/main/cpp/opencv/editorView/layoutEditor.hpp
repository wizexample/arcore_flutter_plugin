//
//  layerEditor.hpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/17.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#ifndef layerEditor_hpp
#define layerEditor_hpp

#include <stdio.h>
#include "../data.hpp"

using namespace cv;


class LayoutEditor {
private:
    bool isReady = false;
    CProject project;
    int currentForegroundId;
    
    void putImage(Mat4b *canvas, const CForeground& foreground, bool enableHalfTransparent);
    void putBackground(Mat4b *canvas);
    void loadBGImage();
    void uniteImages(Mat4b &dest, bool addChecker, bool enableHalfTransprent);

public:
    void initProject(CProject project);
    void close();
    void addForeground(const CForeground foreground);
    void detouchForeground(int targetId);
    void setCurrentForeground(int fgId);
    CForeground* takeForegroundFromId(int targetId);
    void createMaskedImage(Mat4b &dest, CForeground* fg, Mat4b base, const Mat1b& mask);
    void getDisplayImage(Mat4b &dest);
    void getImageForThumbnail(Mat4b &dest);
    void getSmallThumbnail(const Mat4b src, Mat4b &dest, int width, int height, float offset);
    void setBackground(int bgId, Mat4b bgImage);
    void deleteBackground();
    void setForegroundsOrder(int* order, int arraySize);
  
    void updateForeground(int targetId, int z, double x, double y, double xScale, double yScale, double rotate, double toneH, double toneS, double toneV, double contrast, int blurSize, int blurErode);
    void updateForegroundImage(int targetId, Mat4b maskedImage);
    
    void createInitialMask(Mat4b base, Mat1b &dest);
    void imageNormalization(Mat4b src, Mat4b *dest, int degree, int size);
};


#endif /* layerEditor_hpp */
