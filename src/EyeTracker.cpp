//
//  EyeTracker.cpp
//  eyeTracker_videoTest
//
//  Created by Tyler on 2/14/16.
//
//

#include "EyeTracker.hpp"


EyeTracker::EyeTracker(){
    
    // init fbos (ping-pong style)
    fbo1.allocate(ofGetWidth(),ofGetHeight());
    
    fbo1.begin();
        ofClear(0);
    fbo1.end();
    
    fbo1Tex.allocate(fbo1.getTextureReference().getWidth(),fbo1.getTextureReference().getHeight(),GL_RGBA);
    
    fbo2.allocate(ofGetWidth(),ofGetHeight());
    
    fbo2.begin();
        ofClear(0);
    fbo2.end();
    
    fbo2Tex.allocate(fbo2.getTextureReference().getWidth(),fbo2.getTextureReference().getHeight(),GL_RGBA);
    
    // init shaders
    fade.load("","fade.frag");
    blur.load("","blur.frag");
    recenter.load("","recenter.frag");
    
    reset(); // clear eyePositions vector and init current and prev eyepos to center screen
    
}

//--------------------------------------------------------------

void EyeTracker::update(ofVec2f pos, int frame){
    
    if (!(currentEyePos.frame == frame && currentEyePos.pos == pos)){ // if input frame/pos != currentEyePos
    
        prevEyePos = currentEyePos;
        
        currentEyePos.pos = pos;
        currentEyePos.frame = frame;
        
        eyePositions.push_back(currentEyePos);
    }
}

//--------------------------------------------------------------

void EyeTracker::draw(ofTexture & vidTexture, ofVec2f pos, float x, float y, float w, float h, bool drawDot, bool bRecenter, bool bBlink){
    
    if (bBlink || pos.x == 0 || pos.y == 0){
        pos.x = -10000;
        pos.y = -10000;
    }
    
    fbo1Tex = fbo1.getTextureReference();
    fbo2Tex = fbo2.getTextureReference();
    
    // horz blur pass
    fbo1.begin();
        ofClear(0);
        blur.begin();
        
            blur.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            blur.setUniform2f("u_mouse", pos.x, pos.y);
            blur.setUniformTexture("u_tex0", vidTexture, 0);
            blur.setUniform2f("u_tex0Resolution", vidTexture.getWidth(), vidTexture.getHeight());
            blur.setUniform2f("u_dir",1.0,0.0);
            blur.setUniform1i("u_vFlip",0);
    
            ofRect(0,0,ofGetWidth(),ofGetHeight());
    
        blur.end();
    fbo1.end();
    
    // vert blur pass
    fbo2.begin();
        ofClear(0);
        blur.begin();
        
            blur.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            blur.setUniform2f("u_mouse", pos.x, pos.y);
            blur.setUniformTexture("u_tex0", fbo1Tex, 0);
            blur.setUniform2f("u_tex0Resolution", fbo1Tex.getWidth(), fbo1Tex.getHeight());
            blur.setUniform2f("u_dir",0.0,1.0);
            blur.setUniform1i("u_vFlip",0);
    
            ofRect(0,0,ofGetWidth(),ofGetHeight());
        
        blur.end();
    fbo2.end();
    
    fbo2.draw(0,0,ofGetWidth(),ofGetHeight());

    // fade pass
    fbo1.begin();
        ofClear(0);
        fade.begin();
        
            fade.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            fade.setUniform2f("u_mouse", pos.x, pos.y);
            fade.setUniformTexture("u_tex0", fbo2Tex, 0);
            fade.setUniform2f("u_tex0Resolution", fbo2Tex.getWidth(), fbo2Tex.getHeight());
            fade.setUniform1i("u_vFlip",0);
        
            ofRect(0,0,ofGetWidth(),ofGetHeight());
        
        fade.end();
    fbo1.end();
    
    // either draw with recentering shader
    if (bRecenter){
        
        ofClear(0);
        recenter.begin();
        
            recenter.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            recenter.setUniform2f("u_mouse", pos.x, pos.y);
            recenter.setUniformTexture("u_tex0", fbo1Tex, 0);
            recenter.setUniform2f("u_tex0Resolution", fbo1Tex.getWidth(), fbo1Tex.getHeight());
            recenter.setUniform1i("u_vFlip",1);
            
            ofRect(0,0,ofGetWidth(),ofGetHeight());
        
        recenter.end();
        
    }
    // or draw the fade fbo
    else {
        
        fbo1.draw(0,0,ofGetWidth(),ofGetHeight());
    }

    
    if (drawDot){
        ofPushStyle();
        ofSetColor(255,0,0);
        ofNoFill();
        ofCircle(currentEyePos.pos.x, currentEyePos.pos.y, 20);
        ofPopStyle();
    }
    
}

//--------------------------------------------------------------

void EyeTracker::drawCurrent(ofTexture &vidTexture, float x, float y, float w, float h, bool drawDot, bool bRecenter){
    draw(vidTexture, currentEyePos.pos, x,y, w,h, drawDot, bRecenter);
}

//--------------------------------------------------------------

void EyeTracker::replay(ofTexture &vidTexture, int frame, float x, float y, float w, float h, bool drawDot, bool bRecenter, bool bEase){
    
    // get closest saved frame to current frame in video
    currentEyePos = *(findClosestFrame(frame));
    
    ofVec2f pos = currentEyePos.pos;
    
    if (bEase){
        if (currentEyePos.pos.x == 0 || currentEyePos.pos.y == 0){
            pos = ofVec2f(0,0); // blink, don't ease
        } else {
            float rate = 0.1;
            // easing (linear)
            pos = prevEyePos.pos * (1-rate) + pos * rate;
            prevEyePos.pos = pos;
        }
    }
    
    draw(vidTexture, pos, x,y, w,h, drawDot, bRecenter);
    
}

//--------------------------------------------------------------

void EyeTracker::save(int trackCount){
    
    for (int i=0; i<eyePositions.size(); i++){
        
        csv.setInt(i,0, eyePositions[i].frame);
        csv.setFloat(i,1, eyePositions[i].pos.x);
        csv.setFloat(i,2, eyePositions[i].pos.y);
        
    }
    
    string filename = "results_";
    filename.append(ofToString(ofGetTimestampString("%Y_%m_%d__%Hh-%Mm-%Ss"))); //2016_02_17__17h-41m-32s
    filename.append("__run_" + ofToString(trackCount));
    filename.append(".csv");
    csv.saveFile(ofToDataPath(filename));
    
    ofLogNotice("EyeTracker","saved data as " + filename);
    
    csv.clear(); //clear csv data
    
}

//--------------------------------------------------------------

void EyeTracker::load(string filename){
    
    loaded = false;
    
    eyePositions.clear();
    
    csv.loadFile(ofToDataPath(filename));
    
    // load eyePositions vector
    ofLogNotice("EyeTracker", "loading csv " + filename);
    
    for (int row=0; row<csv.numRows; row++){
        EyePos ep;
        ep.frame = csv.getInt(row,0);
        ep.pos = ofVec2f(csv.getFloat(row,1), csv.getFloat(row,2));
        
        eyePositions.push_back(ep);
    }
    
    loaded = true;
    
}

//--------------------------------------------------------------

void EyeTracker::reset(){
    
    eyePositions.clear();
    
    // reset current and prev eyepos
    currentEyePos.pos = ofVec2f(ofGetWidth()*0.5, ofGetHeight()*0.5); // center screen
    currentEyePos.frame = 0;
    prevEyePos = currentEyePos;
    
}

//--------------------------------------------------------------

ofVec2f EyeTracker::getPosAtFrame(int frame){
    
    EyePos * foundFrame = findClosestFrame(frame);
    return foundFrame->pos;
    
}

//--------------------------------------------------------------

EyeTracker::EyePos * EyeTracker::findClosestFrame(int frame){
    
    EyePos findFrame;
    findFrame.pos = ofVec2f(0,0);
    findFrame.frame = frame;
    
    //find first element greater than
    vector<EyePos>::iterator it =
        upper_bound(eyePositions.begin(),eyePositions.end(),findFrame,FrameComp());
    
    //return frame before result as pointer to EyePos
    if (it-1 >= eyePositions.begin()){
        return &(*(--it));
    }
    return &(*(eyePositions.begin())); //else return begin() as pointer
    
}