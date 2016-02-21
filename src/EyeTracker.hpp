//
//  EyeTracker.hpp
//  eyeTracker_videoTest
//
//  Created by Tyler on 2/14/16.
//
//

#pragma once 
#include "ofMain.h"
#include "ofxCsv.h"

using namespace wng; //csv

class EyeTracker {
    
private:
    
    struct EyePos {
        ofVec2f pos;
        int frame;
    };
    
    vector<EyePos> eyePositions;
    
    struct FrameComp {
        bool operator()(const EyePos& frame1, const EyePos& frame2){
            return frame1.frame < frame2.frame;
        }
    };
    
    EyePos prevEyePos; // stores eyepos from previous frame
    
public:
    
    EyeTracker();
    
    void update(ofVec2f pos, int frame); // add eye tracking position
    void draw(ofTexture & vidTexture, ofVec2f pos, float x, float y, float w, float h, bool drawDot = false, bool bRecenter = false, bool bBlink = false);
    
    void drawCurrent(ofTexture & vidTexture, float x, float y, float w, float h, bool drawDot = false, bool bRecenter = false);
    void replay(ofTexture & vidTexture, int frame, float x, float y, float w, float h, bool drawDot = false, bool bRecenter = false, bool bEase = false);
    
    void save(int trackCount);
    void load(string filename);
    void reset();
    
    ofVec2f getPosAtFrame(int frame); // return eye tracking position
    
    EyePos * findClosestFrame(int frame); // finds frame # <= input
    
    EyePos currentEyePos;
    ofFbo fbo1, fbo2;
    ofTexture fbo1Tex, fbo2Tex;
    ofShader fade;
    ofShader blur;
    ofShader recenter;
    
    ofxCsv csv; //csv for saving data
    bool loaded = false;
    
};
