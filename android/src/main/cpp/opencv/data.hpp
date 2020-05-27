//
//  data.hpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/17.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#ifndef data_hpp
#define data_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// DBと直結するわけではないのですべてのカラムを管理する必要はない
// 必要な情報だけ持つ 必要ならDBにない情報(→Matなど)も持つ
class CForeground {
private:
    
public:
    // columns fileds
    int id;
    int projectId;
    string image;
    string mask;
    int zIndex;
    double xScale;
    double yScale;
    double xAxis;
    double yAxis;
    double rotate;
    double toneH;
    double toneS;
    double toneV;
    double contrast;
    int blurSize;
    int blurErode;

    // local fields
    Mat4b maskedImage;
    
    // methods
    string description();
};

class CProject {
private:
    static bool comparator(const CForeground &l, const CForeground &r);
    
public:
    // columns fileds
    int id;
    String name;
    int bgId;
    int mainImageId;
    int uploaded;
    String thumbnail;
    int folderId;

    // fileds
    Size2i canvasSize;
    vector<CForeground> foregrounds;
    Mat4b background;
    
    // methods
    string description();
    void sort();
};

class CBackground {
    int id;
    String path;
    
    string description();
};

class OperationHistory {
private:
    int id;
public:
    Mat1b mask;
    bool isChromakey = false; // T: Chromakey F: Draw
    Vec3b chromakeyHSV;
    bool isErase = true; // T: Erase F: Fallback / if in Chromakey mode, always Erase
    
    OperationHistory();
    string description();
};

#endif /* data_hpp */
