//
//  data.cpp
//  Runner
//
//  Created by 上江洲　智久 on 2019/09/17.
//  Copyright © 2019 The Chromium Authors. All rights reserved.
//

#include "data.hpp"


string CForeground::description() {
    stringstream ss;
    ss<<"id: "<<id<<", pId: "<<projectId<<", z: "<<zIndex<<", axis: ["<<xAxis<<", "<<yAxis<<"], scale: ["<<xScale<<", "<<yScale<<"], rotate: "<<rotate<<", H: "<<toneH<<", S: "<<toneS<<", V: "<<toneV<<", contrast: "<<contrast;
    return ss.str();
}

string CProject::description() {
    stringstream ss;
    ss<<"id: "<<id<<", name: "<<name<<", bgId: "<<bgId<<", imgId: "<<mainImageId<<", uped: "<<uploaded<<", fId:"<<folderId;
    return ss.str();
}

string CBackground::description() {
    stringstream ss;
    ss << "id: " << id << ", path: " << path << endl;
    return ss.str();
}

void CProject::sort() {
    std::sort(foregrounds.begin(), foregrounds.end(), comparator);
}

bool CProject::comparator(const CForeground &l, const CForeground &r) {
    return l.zIndex > r.zIndex;
}

static int counter = 0;
OperationHistory::OperationHistory() {
    this->id = counter;
    counter ++;
}

string OperationHistory::description() {
    stringstream ss;
    ss << "history id:" << id<< endl;
    return ss.str();
}
