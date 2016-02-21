#pragma once

#include "ofMain.h"
#include "ofxEyeTribe.h"
#include "EyeTracker.hpp"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void play();
    void calibrate();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    
    ofxEyeTribe tet;
    EyeTracker tracker;
    
    int w,h; //video w,h
    
    ofVideoPlayer vid;
    int vidFrame = 0;
    
    bool calibrated = true;
    bool drawTracker = false;
    bool drawTrackerDot = false;
    bool recenter = false;
    bool ease = false;
    bool vidDone = false;
    
    bool tracking = true; // !tracking = replaying
    bool waiting = true; // waiting for input
    bool loading = false; // loading from csv
    bool playFromFile = false; // playing from csv file
    
    ofImage track;
    ofImage replay;
    
    int trackCount = 0;
    
    ofDirectory dir;
    int selection = 0;
    
};
